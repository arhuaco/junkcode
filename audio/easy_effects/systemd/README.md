Easyeffects @ background

Mostly a local copy for my use, taken from:
  https://github.com/wwmm/easyeffects/wiki/Headless-Operation
Their content is GPLv3, retrieved 2025-5-25.
https://github.com/wwmm/easyeffects/blob/master/LICENSE


Store the following file in .config/systemd/user/easyeffects-xvfb.service :

```
[Unit]
Description=EasyEffects inside Xvfb

[Service]
Type=simple
ExecStart=/usr/local/bin/easyeffects-xvfb start
ExecStop=/usr/local/bin/easyeffects-xvfb stop
Restart=on-failure

[Install]
WantedBy=default.target
```

Some commands:


```
systemctl --user daemon-reload
systemctl --user start easyeffects-xvfb
systemctl --user enable easyeffects-xvfb
```


