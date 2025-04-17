import { Component } from '@angular/core';
import { CommonModule } from '@angular/common';
import { ValvePanelComponent } from './components/valve-panel/valve-panel.component';

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [CommonModule, ValvePanelComponent],
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent {}
