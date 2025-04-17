import { Component, Input } from '@angular/core';
import { CommunicationService } from '../../services/communication.service';

@Component({
  selector: 'app-valve-button',
  standalone: true,
  imports: [],
  templateUrl: './valve-button.component.html',
  styleUrl: './valve-button.component.scss'
})
export class ValveButtonComponent {

  @Input() valveIndex: number = 0;
  @Input() valveState: boolean = false;

  constructor(
    private communication: CommunicationService
  ) {}

  toggle() {
    if (this.valveState) {
      this.communication.disableActiveValve();
      return;
    }

    this.communication.enableValveByIndex(this.valveIndex);
  }
}
