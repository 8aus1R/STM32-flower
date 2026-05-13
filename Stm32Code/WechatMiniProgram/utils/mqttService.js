const mqtt = require("mqtt/dist/mqtt.min");
const mqttConfig = require("../config/mqttConfig");

const MAX_LOGS = 20;

let client = null;
let connectStatus = "disconnected";
let deviceStatus = "unknown";
let latestData = {
  soil: "--",
  light: "--",
  temperature: "--",
  humidity: "--"
};
let logs = [];
let listeners = [];

function nowText() {
  const date = new Date();
  const h = String(date.getHours()).padStart(2, "0");
  const m = String(date.getMinutes()).padStart(2, "0");
  const s = String(date.getSeconds()).padStart(2, "0");
  return `${h}:${m}:${s}`;
}

function pushLog(message) {
  logs.unshift(`[${nowText()}] ${message}`);
  logs = logs.slice(0, MAX_LOGS);
  notify();
}

function notify() {
  const snapshot = getState();
  listeners.forEach((listener) => {
    if (typeof listener === "function") {
      listener(snapshot);
    }
  });
}

function getBrokerUrl() {
  if (mqttConfig.host.indexOf("://") !== -1) {
    return mqttConfig.host;
  }

  return `${mqttConfig.protocol}://${mqttConfig.host}:${mqttConfig.port}${mqttConfig.path}`;
}

function normalizeData(payload) {
  const data = typeof payload === "string" ? JSON.parse(payload) : payload;

  latestData = {
    soil: pickValue(data, ["soil", "soilHumidity", "soil_moisture", "soilMoisture", "sensor2"], latestData.soil),
    light: pickValue(data, ["light", "lightIntensity", "light_intensity", "sensor3"], latestData.light),
    temperature: pickValue(data, ["temperature", "temp", "sensor1"], latestData.temperature),
    humidity: pickValue(data, ["humidity", "humi", "airHumidity"], latestData.humidity)
  };
}

function pickValue(source, keys, fallback) {
  for (let i = 0; i < keys.length; i += 1) {
    const key = keys[i];
    if (source[key] !== undefined && source[key] !== null && source[key] !== "") {
      return source[key];
    }
  }
  return fallback;
}

function handleMessage(topic, message) {
  const text = message.toString();

  if (topic === mqttConfig.topics.data) {
    try {
      normalizeData(text);
      pushLog(`received data ${text}`);
    } catch (error) {
      pushLog(`data parse failed ${text}`);
    }
    notify();
    return;
  }

  if (topic === mqttConfig.topics.status) {
    deviceStatus = text;
    pushLog(`device status ${text}`);
    notify();
    return;
  }

  pushLog(`received message ${topic}: ${text}`);
}

function connect() {
  if (client && (connectStatus === "connecting" || connectStatus === "connected")) {
    return;
  }

  connectStatus = "connecting";
  pushLog("connecting MQTT");

  client = mqtt.connect(getBrokerUrl(), {
    clientId: mqttConfig.clientId,
    username: mqttConfig.username,
    password: mqttConfig.password,
    clean: true,
    connectTimeout: 8000,
    reconnectPeriod: 3000,
    keepalive: 60,
    protocolVersion: 4
  });

  client.on("connect", () => {
    connectStatus = "connected";
    pushLog("connected");
    client.subscribe([mqttConfig.topics.data, mqttConfig.topics.status], { qos: 0 }, (error) => {
      if (error) {
        pushLog(`subscribe failed ${error.message || error}`);
      } else {
        pushLog("subscribe data and status topics success");
      }
    });
    notify();
  });

  client.on("reconnect", () => {
    connectStatus = "connecting";
    pushLog("reconnecting");
    notify();
  });

  client.on("close", () => {
    if (connectStatus !== "disconnected") {
      connectStatus = "disconnected";
      pushLog("connection closed");
      notify();
    }
  });

  client.on("error", (error) => {
    connectStatus = "error";
    pushLog(`connection failed ${error.message || error}`);
    notify();
  });

  client.on("message", handleMessage);
}

function disconnect() {
  if (!client) {
    return;
  }

  client.end(true);
  client = null;
  connectStatus = "disconnected";
  pushLog("disconnected");
  notify();
}

function publishCommand(commandObject) {
  const payload = JSON.stringify(commandObject);

  if (!client || connectStatus !== "connected") {
    pushLog(`publish failed, MQTT not connected ${payload}`);
    wx.showToast({
      title: "MQTT not connected",
      icon: "none"
    });
    return;
  }

  client.publish(mqttConfig.topics.command, payload, { qos: 0, retain: false }, (error) => {
    if (error) {
      pushLog(`publish failed ${payload}`);
      wx.showToast({
        title: "publish failed",
        icon: "none"
      });
    } else {
      pushLog(`published command ${payload}`);
    }
    notify();
  });
}

function onChange(listener) {
  listeners.push(listener);
  listener(getState());

  return function unsubscribe() {
    listeners = listeners.filter((item) => item !== listener);
  };
}

function getState() {
  return {
    connectStatus,
    deviceStatus,
    latestData,
    logs
  };
}

module.exports = {
  connect,
  disconnect,
  publishCommand,
  onChange,
  getState
};
