const mqttService = require("../../utils/mqttService");

Page({
  data: {
    statusText: "Disconnected",
    statusClass: "",
    deviceStatus: "unknown",
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

  sendCommand(event) {
    const cmd = event.currentTarget.dataset.cmd;
    mqttService.publishCommand({ cmd });
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
      logs: state.logs
    });
  }
});
