import { Injectable } from '@angular/core';
import { BehaviorSubject } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class CommunicationService {

  private WPC_ENABLE_VALVE_BY_INDEX = 0;
  private WPC_DISABLE_ACTIVE_VALVE  = 1;
  private WPC_VALVES_STATE_CHANGE   = 2;

  public valveStates$ = new BehaviorSubject<boolean[]>([]);

  // TODO: Change me
  // Since this little web-app is served using the ESP32 using an SD-card, the
  // address can be determined via window.location - use static values only in a
  // development environment, when serving locally.
  private serverUrl = "http://192.168.178.54:80/ws"
  private socket: WebSocket;

  constructor() {
    this.socket = this.setupSocket();
  }

  // TODO: Detect timeouts - maybe include a heartbeat? That's probably the only proper solution.

  private setupSocket(): WebSocket {
    const socket = new WebSocket(this.serverUrl);

    socket.onopen = () => {
      console.log("Socket opened");
    };

    socket.onclose = () => {
      console.log("Socket closed");
      // TODO: Auto-reconnect!
    };

    socket.onerror = error => {
      console.error("Socket error", error);
    };

    socket.onmessage = event => {
      var reader = new FileReader();
      reader.readAsArrayBuffer(event.data);

      reader.onloadend = () => {
        const data = new Uint8Array(reader.result as ArrayBuffer);

        if (data.length == 0)
          return;

        this.onSocketDataReceive(data);
      };
    };

    return socket;
  }

  enableValveByIndex(index: number) {
    if (index >= 0)
      this.sendSocketData(new Uint8Array([this.WPC_ENABLE_VALVE_BY_INDEX, index]));
  }

  disableActiveValve() {
    this.sendSocketData(new Uint8Array([this.WPC_DISABLE_ACTIVE_VALVE]));
  }

  private sendSocketData(data: Uint8Array) {
    this.socket.send(data);
  }

  private onSocketDataReceive(data: Uint8Array) {
    if (data[0] == this.WPC_VALVES_STATE_CHANGE) {
      if (data.length == 1)
        return;

      const valveCount = data[1];
      const bitpackedBytesCount = Math.ceil(valveCount / 8);

      if (data.length != 2 + bitpackedBytesCount)
        return;

      const states = Array(valveCount).fill(false);

      let valveIndex = 0;
      for (let byteIndex = 0; byteIndex < bitpackedBytesCount; ++byteIndex) {
        const currentByte = data[2 + byteIndex];

        for (let bitIndex = 7; bitIndex >= 0; --bitIndex) {
          if (valveIndex >= valveCount)
            break;

          states[valveIndex++] = currentByte & (1 << bitIndex);
        }
      }

      this.valveStates$.next(states);
    }
  }
}
