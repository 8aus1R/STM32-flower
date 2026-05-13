# STM32 自动浇花系统微信小程序调试端

这是一个原生微信小程序，用于通过公共测试 Broker 调试 STM32 自动浇花系统。

## 当前第一阶段 MQTT 方案

当前不使用 EMQX Cloud Serverless 给 STM32 连接普通 TCP，因为 EMQX Cloud Serverless 不支持 1883 普通 MQTT TCP，只支持 8883 MQTT TLS 和 8084 WSS。

第一阶段统一使用公共测试 Broker：

```text
broker.emqx.io
```

微信小程序连接：

```text
wss://broker.emqx.io:8084/mqtt
```

STM32 + ESP8266 连接：

```text
broker.emqx.io
端口 1883
普通 MQTT TCP
```

STM32 不使用 WSS，不使用 TLS，不使用 8084。

## Topic 和命令

- 小程序发布命令到：`flower/device001/cmd`
- STM32 订阅命令：`flower/device001/cmd`
- 小程序订阅数据：`flower/device001/data`
- 小程序订阅状态：`flower/device001/status`

控制命令：

```json
{"cmd":"pump_on"}
```

```json
{"cmd":"pump_off"}
```

## MQTT 配置

配置集中在 `config/mqttConfig.js`：

```js
host: "wss://broker.emqx.io:8084/mqtt"
username: ""
password: ""
clientId: "wechat_flower_" + Date.now()
```

## 目录结构

```text
WechatMiniProgram/
  app.js
  app.json
  app.wxss
  config/
    mqttConfig.js
  pages/
    index/
    control/
  utils/
    mqttService.js
  package.json
  project.config.json
  sitemap.json
```

## 如何运行

1. 安装微信开发者工具。
2. 用微信开发者工具打开 `WechatMiniProgram` 目录。
3. 选择“小程序”项目类型。
4. AppID 可先使用测试号或游客模式。
5. 在微信开发者工具右上角打开：

```text
详情 -> 本地设置 -> 不校验合法域名、web-view（业务域名）、TLS 版本以及 HTTPS 证书
```

6. 在 `WechatMiniProgram` 目录执行：

```bash
npm install
```

7. 回到微信开发者工具，点击：

```text
工具 -> 构建 npm
```

8. 点击“编译”运行小程序。

## 数据格式建议

小程序收到 `flower/device001/data` 后会尝试解析 JSON。推荐 STM32 上传：

```json
{
  "soil": 45,
  "light": 60,
  "temperature": 28,
  "humidity": 52
}
```

设备状态 Topic `flower/device001/status` 可以发送普通文本，例如：

```text
online
offline
manual
auto
```
