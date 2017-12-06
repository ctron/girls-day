/********************************************************************************
* Copyright (c) 2017 Red Hat Inc and others
*
* This program and the accompanying materials are made available under the
* terms of the Eclipse Public License 2.0 which is available at
* http://www.eclipse.org/legal/epl-2.0.
*
* This Source Code may also be made available under the following Secondary
* Licenses when the conditions for such availability set forth in the Eclipse
* Public License, v. 2.0 are satisfied: GNU General Public License, version 2
* with the GNU Classpath Exception which is
* available at https://www.gnu.org/software/classpath/license.html.
*
* SPDX-License-Identifier: EPL-2.0 OR GPL-2.0 WITH Classpath-exception-2.0
********************************************************************************/
package de.dentrassi.iot;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Random;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import io.vertx.core.Vertx;
import io.vertx.mqtt.MqttClient;
import io.vertx.mqtt.messages.MqttPublishMessage;

public class GirlsDayServer implements Server {

    public static final String TOPIC_PREFIX = "/GirlsDay2017/redhat/munich/";

    private static final Logger logger = LoggerFactory.getLogger(GirlsDayServer.class);

    private final Vertx vertx;

    private final MqttClient client;

    private final Map<String, DeviceState> devices = new HashMap<>();

    private GameState state = GameState.INACTIVE;

    private List<DeviceState> players;

    private final Random random = new Random();

    private long timer;

    public GirlsDayServer(final Vertx vertx) {

        this.vertx = vertx;
        this.client = MqttClient.create(vertx)
                .publishHandler(this::handleMessage)
                .closeHandler(x -> triggerConnect());

        vertx.setPeriodic(1_000, t -> {
            if (this.client.isConnected()) {
                this.client.ping();
            }
        });

        vertx.setPeriodic(5_000, t -> {
            checkStart();
        });

        triggerConnect();
    }

    private void checkStart() {
        if (this.state != GameState.INACTIVE) {
            // already playing
            return;
        }

        if (this.devices.size() <= 0) {
            logger.debug("Waiting for players");
            return;
        }

        startGame();
    }

    private void handleMessage(final MqttPublishMessage message) {

        String topic = message.topicName();

        logger.debug("{} -> {}", topic, message.payload());

        if (!topic.startsWith(TOPIC_PREFIX)) {
            return;
        }

        topic = topic.substring(TOPIC_PREFIX.length());

        final LinkedList<String> tokens = new LinkedList<>(Arrays.asList(topic.split("\\/")));

        final String deviceId = tokens.pollFirst();
        if (tokens.size() != 1) {
            return;
        }

        final String what = tokens.get(0);
        switch (what) {
        case "clicked":
            deviceClicked(deviceId);
            break;
        case "client":
            clientChanged(deviceId, Integer.parseInt(message.payload().toString()));
            break;
        }

    }

    private void clientChanged(final String deviceId, final int state) {
        logger.debug("{}: STATE {}", deviceId, state);

        DeviceState device = this.devices.get(deviceId);

        if (device == null && state == 1) {
            logger.debug("Player {} joined", deviceId);
            device = new DeviceState(deviceId, this, this.client);
            this.devices.put(deviceId, device);
        } else if (device != null && state == 0) {
            logger.debug("Player {} left", deviceId);
            this.devices.remove(deviceId);
        }
    }

    private void deviceClicked(final String deviceId) {
        logger.debug("{}: CLICKED", deviceId);

        final DeviceState device = this.devices.get(deviceId);

        if (device == null) {
            logger.warn("Unknown device clicked: {}", deviceId);
            return;
        }

        device.clicked();
    }

    private void triggerConnect() {
        this.client.connect(1883, "iot.eclipse.org", connected -> {
            if (connected.failed()) {
                triggerConnect();
            } else {
                startServer();
            }
        });
    }

    private void startServer() {
        logger.info("Connected...");
        this.client.subscribe("/GirlsDay2017/redhat/munich/#", 0);
    }

    private void startGame() {
        logger.info("Starting game");

        this.state = GameState.WAITING_FOR_PARTICIPANTS;

        for (final DeviceState device : this.devices.values()) {
            device.startJoining();
        }

        this.vertx.setTimer(5_000, t -> begin());
    }

    private void begin() {

        logger.debug("Beginning game");

        this.state = GameState.PLAYING;

        this.players = new ArrayList<>();

        for (final Map.Entry<String, DeviceState> entry : this.devices.entrySet()) {

            final DeviceState device = entry.getValue();
            device.stopJoining();

            if (device.didJoin()) {
                logger.info("{} is playing", entry.getKey());
                this.players.add(device);
                device.plays(true);
            } else {
                logger.info("{} is not playing", entry.getKey());
                device.plays(false);
            }
        }

        if (this.players.isEmpty()) {
            logger.debug("No one joined");
            this.state = GameState.INACTIVE;
            return;
        }

        selectNext();
    }

    private void selectNext() {

        final int selected = this.random.nextInt(this.players.size());

        logger.info("Selected player: {}", selected);
        final DeviceState device = this.players.get(selected);
        device.select();

        this.timer = this.vertx.setTimer(2_000, t -> selectionTimeout());
    }

    private void selectionTimeout() {
        this.timer = -1;
        gameOver();
    }

    private void gameOver() {
        this.state = GameState.GAME_OVER;

        logger.info("Game over!");

        this.devices.values().forEach(DeviceState::gameOver);

        stopTimeout();

        this.vertx.setTimer(5_000, t -> {
            this.state = GameState.INACTIVE;
            checkStart();
        });
    }

    @Override
    public void playerFail(final DeviceState deviceState) {
        gameOver();
    }

    private void stopTimeout() {
        if (this.timer >= 0) {
            this.vertx.cancelTimer(this.timer);
            this.timer = -1;
        }
    }

    @Override
    public void playerSuccess(final DeviceState deviceState) {
        stopTimeout();

        this.vertx.setTimer(1_000, t -> {
            selectNext();
        });
    }

    private void run() throws InterruptedException {
        Thread.sleep(Long.MAX_VALUE);
    }

    public static void main(final String[] args) throws InterruptedException {

        System.setProperty("vertx.logger-delegate-factory-class-name", "io.vertx.core.logging.SLF4JLogDelegateFactory");

        final Vertx vertx = Vertx.vertx();
        try {
            new GirlsDayServer(vertx).run();
        } finally {
            vertx.close();
        }
    }

}
