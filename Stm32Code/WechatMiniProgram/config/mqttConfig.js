const mqttConfig = {
  host: "wss://broker.emqx.io:8084/mqtt",
  username: "",
  password: "",
  clientId: "wechat_flower_" + Date.now(),

  topics: {
    command: "flower/device001/cmd",
    data: "flower/device001/data",
    status: "flower/device001/status"
  }
};

module.exports = mqttConfig;
