To create a new Fedora Girls Day spin:

- clone this repo
- download the rpms in the extra_repo
- run the following command in the repo:
    createrepo_c "$PWD"/extra_repo/
- you need a Fedora install of the version you want to build the ISO for
    sudo yum -y install livecd-tools fedora-kickstarts vim liveusb-creator

# create live ISO
    time sudo livecd-creator --cache=/root/livecd-creator_cache/ -c /root/fedora-girlsday-respin/fedora-live-workstation.ks --fslabel=F28-x86_64-LIVEGIRLSDAY


# create persistent partition and write to USB stick
    umount /dev/sdb1
# time sudo livecd-iso-to-disk --format --efi --resetmbr --home-size-mb 1000 --unencrypted-home --label "GirlsDay" F27-x86_64-LIVEGIRLSDAY.iso /dev/sdb
    time sudo livecd-iso-to-disk --format --resetmbr --home-size-mb 1000 --unencrypted-home --label "GirlsDay" F28-x86_64-LIVEGIRLSDAY.iso /dev/sdb