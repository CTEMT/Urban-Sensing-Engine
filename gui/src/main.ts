import { AppComponent, BrandComponent, Connection, Settings } from '@ratiosolver/flick';
import { coco } from '@ratiosolver/coco';
import { Offcanvas as USPEOffcanvas } from './offcanvas';
import { TemperatureComponent } from './temperature';
import './style.css'
import { Offcanvas } from 'bootstrap';

Settings.get_instance().load_settings({ ws_path: '/coco' });

const offcanvas_id = 'restart-offcanvas';

class USPEBrandComponent extends BrandComponent {

  constructor() {
    super('Urban Sensing & Planning Engine', 'logo.png', 120, 40);
    this.node.id = 'uspe-brand';
  }
}

class USPEApp extends AppComponent implements coco.CoCoListener, coco.taxonomy.ItemListener {

  private temperature_component: TemperatureComponent;
  private temperature_sensors: Set<coco.taxonomy.Item> = new Set();

  constructor() {
    super(new USPEBrandComponent());

    this.temperature_component = new TemperatureComponent();
    this.add_child(this.temperature_component);

    const offcanvas = new USPEOffcanvas(offcanvas_id);
    this.add_child(offcanvas);
    document.getElementById('uspe-brand')?.addEventListener('click', (event) => {
      event.preventDefault();
      Offcanvas.getOrCreateInstance(document.getElementById(offcanvas_id)!).toggle();
    });

    coco.CoCo.get_instance().add_coco_listener(this);
    Connection.get_instance().connect();
  }

  new_type(_: coco.taxonomy.Type): void { }
  new_item(item: coco.taxonomy.Item): void {
    if (Array.from(item.get_types()).some(t => t.get_name() === 'EnvironmentalSensor')) {
      this.temperature_sensors.add(item);
      item.add_item_listener(this);
      coco.CoCo.get_instance().load_data(item);
    }
  }
  types_updated(item: coco.taxonomy.Item): void {
    if (Array.from(item.get_types()).some(t => t.get_name() === 'EnvironmentalSensor')) {
      if (!this.temperature_sensors.has(item)) {
        this.temperature_sensors.add(item);
        item.add_item_listener(this);
      }
    } else {
      if (this.temperature_sensors.has(item)) {
        this.temperature_sensors.delete(item);
        item.remove_item_listener(this);
      }
    }
  }
  properties_updated(_: coco.taxonomy.Item): void { }
  values_updated(item: coco.taxonomy.Item): void {
    if (this.temperature_sensors.has(item)) {
      item.get_datum
    }
  }
  new_value(item: coco.taxonomy.Item, v: coco.taxonomy.Datum): void {
    if (this.temperature_sensors.has(item) && v.data['temperature'] !== undefined)
      this.temperature_component.set_temperature(v.data['temperature'] as number);
  }

  override received_message(message: any): void { coco.CoCo.get_instance().update_coco(message); }

  override connection_error(_error: any): void { this.toast('Connection error. Please try again later.'); }

  override unmounting(): void {
    super.unmounting();
    for (const sensor of this.temperature_sensors)
      sensor.remove_item_listener(this);
    coco.CoCo.get_instance().remove_coco_listener(this);
  }
}

new USPEApp();