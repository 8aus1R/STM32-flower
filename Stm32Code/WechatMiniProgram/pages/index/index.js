const mqttService = require("../../utils/mqttService");

Page({
  data: {
    statusText: "Disconnected",
    statusClass: "",
    deviceStatus: "unknown",
    latestData: {
      soil: "--",
      light: "--",
      temperature: "--",
      humidity: "--"
    },
    logs: []
  },

  onLoad() {
    this.unsubscribe = mqttService.onChange((state) => {
      this.applyState(state);
    });
    mqttService.connect();
  },

  onUnload() {
    if (this.unsubscribe) {
      this.unsubscribe();
    }
  },

  handleConnect() {
    mqttService.connect();
  },

  handleDisconnect() {
    mqttService.disconnect();
  },

  applyState(state) {
    const statusMap = {
      connected: { text: "Connected", className: "online" },
      connecting: { text: "Connecting", className: "" },
      disconnected: { text: "Disconnected", className: "" },
      error: { text: "Connection error", className: "error" }
    };
    const status = statusMap[state.connectStatus] || statusMap.disconnected;

    this.setData({
      statusText: status.text,
      statusClass: status.className,
      deviceStatus: state.deviceStatus,
      latestData: state.latestData,
      logs: state.logs
    });
  }
});
