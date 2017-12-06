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

import static de.dentrassi.iot.GirlsDayServer.TOPIC_PREFIX;
import static io.vertx.core.buffer.Buffer.buffer;
import static java.lang.String.format;

import io.netty.handler.codec.mqtt.MqttQoS;
import io.vertx.mqtt.MqttClient;

public class DeviceState {
    private final String id;

    private final Server server;

    private final MqttClient connection;

    private boolean joining;
    private boolean selected;
    private boolean joined;

    private boolean plays;

    public DeviceState(final String id, final Server server, final MqttClient connection) {
        this.id = id;
        this.server = server;
        this.connection = connection;
        sendState(0);
    }

    public String getId() {
        return this.id;
    }

    public void clicked() {
        if (this.joining) {
            this.joined = true;
            sendState(2);
            return;
        }

        if (!this.plays) {
            return;
        }

        if (this.selected) {
            success();
        } else {
            fail();
        }
    }

    private void success() {
        this.selected = false;
        sendState(1);
        this.server.playerSuccess(this);
    }

    private void fail() {
        this.server.playerFail(this);
    }

    public void startJoining() {
        this.joined = false;
        this.joining = true;
        sendState(1);
    }

    public void stopJoining() {
        this.joining = false;
        sendState(1);
    }

    public boolean didJoin() {
        return this.joined;
    }

    public void select() {
        this.selected = true;
        sendState(3);
    }

    public void plays(final boolean plays) {
        this.plays = plays;
    }

    public void gameOver() {
        this.selected = false;
        sendState(0);
    }

    private void sendState(final int state) {
        this.connection.publish(
                format("%s%s/server", TOPIC_PREFIX, this.id),
                buffer(Integer.toString(state)),
                MqttQoS.AT_LEAST_ONCE, false,
                false);
    }
}
