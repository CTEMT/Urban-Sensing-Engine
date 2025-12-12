import { AppComponent, BrandComponent, Connection, Settings } from '@ratiosolver/flick';
import { coco } from '@ratiosolver/coco';
import { Offcanvas as USPEOffcanvas } from './offcanvas';
import { Offcanvas } from 'bootstrap';
import './style.css'

Settings.get_instance().load_settings({ ws_path: '/coco' });

const offcanvas_id = 'uspe-offcanvas';

class USPEBrandComponent extends BrandComponent {

  constructor() {
    super('Urban Sensing & Planning Engine', 'logo.png', 120, 40);
    this.node.id = 'uspe-brand';
  }
}

class USPEApp extends AppComponent {

  constructor() {
    super(new USPEBrandComponent());

    const offcanvas = new USPEOffcanvas(offcanvas_id);
    this.add_child(offcanvas);
    document.getElementById('uspe-brand')?.addEventListener('click', (event) => {
      event.preventDefault();
      Offcanvas.getOrCreateInstance(document.getElementById(offcanvas_id)!).toggle();
    });

    Connection.get_instance().connect();
  }

  override received_message(message: any): void { coco.CoCo.get_instance().update_coco(message); }

  override connection_error(_error: any): void { this.toast('Connection error. Please try again later.'); }
}

new USPEApp();