import { Component } from '@angular/core';
import { CommunicationService} from '../../services/communication.service';
import { Observable } from 'rxjs';
import { ValveButtonComponent } from '../valve-button/valve-button.component';
import { AsyncPipe, NgFor } from '@angular/common';

@Component({
  selector: 'app-valve-panel',
  standalone: true,
  imports: [ValveButtonComponent, AsyncPipe, NgFor],
  templateUrl: './valve-panel.component.html',
  styleUrl: './valve-panel.component.scss'
})
export class ValvePanelComponent {

  constructor(
    private communication: CommunicationService
  ) {}

  getValveStates(): Observable<boolean[]> {
    return this.communication.valveStates$;
  }
}
