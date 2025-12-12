import { App, Component, SelectorGroup, type Selector } from "@ratiosolver/flick";
import { TemperatureComponent } from "./temperature";
import { WindComponent } from "./wind";
import { coco } from "@ratiosolver/coco";
import { library, icon } from '@fortawesome/fontawesome-svg-core'
import { faHome } from '@fortawesome/free-solid-svg-icons'

library.add(faHome);

export class HomeElement extends Component<HTMLLIElement> implements Selector {

  private group: SelectorGroup;
  private a: HTMLAnchorElement;

  constructor(group: SelectorGroup) {
    super(document.createElement('li'));
    this.group = group;
    this.node.classList.add('nav-item', 'list-group-item');

    this.a = document.createElement('a');
    this.a.classList.add('nav-link', 'd-flex', 'align-items-center');
    this.a.href = '#';
    const icn = icon(faHome).node[0];
    icn.classList.add('me-2');
    this.a.append(icn);
    this.a.append(document.createTextNode('Home'));
    this.a.addEventListener('click', (event) => {
      event.preventDefault();
      group.set_selected(this);
    });

    this.node.append(this.a);
    group.add_selector(this);
  }

  override unmounting(): void { this.group.remove_selector(this); }

  select(): void {
    this.a.classList.add('active');
    App.get_instance().selected_component(new HomeComponent());
  }
  unselect(): void { this.a.classList.remove('active'); }
}

export class HomeComponent extends Component<HTMLDivElement> implements coco.CoCoListener, coco.taxonomy.ItemListener {

  private readonly temperature_component: TemperatureComponent;
  private readonly wind_direction_component: WindComponent;
  private readonly env_sensors: Set<coco.taxonomy.Item> = new Set();

  constructor() {
    super(document.createElement('div'));
    this.temperature_component = new TemperatureComponent();
    this.wind_direction_component = new WindComponent();
    this.add_child(this.temperature_component);
    this.add_child(this.wind_direction_component);

    coco.CoCo.get_instance().add_coco_listener(this);
    for (const item of coco.CoCo.get_instance().get_items())
      this.new_item(item);
  }

  new_type(_: coco.taxonomy.Type): void { }
  new_item(item: coco.taxonomy.Item): void {
    if (Array.from(item.get_types()).some(t => t.get_name() === 'EnvironmentalSensor')) {
      this.env_sensors.add(item);
      item.add_item_listener(this);
      coco.CoCo.get_instance().load_data(item);
    }
  }
  types_updated(item: coco.taxonomy.Item): void {
    if (Array.from(item.get_types()).some(t => t.get_name() === 'EnvironmentalSensor')) {
      if (!this.env_sensors.has(item)) {
        this.env_sensors.add(item);
        item.add_item_listener(this);
      }
    } else {
      if (this.env_sensors.has(item)) {
        this.env_sensors.delete(item);
        item.remove_item_listener(this);
      }
    }
  }
  properties_updated(_: coco.taxonomy.Item): void { }
  values_updated(item: coco.taxonomy.Item): void {
    if (this.env_sensors.has(item)) {
      item.get_datum
    }
  }
  new_value(item: coco.taxonomy.Item, v: coco.taxonomy.Datum): void {
    if (this.env_sensors.has(item)) {
      if (v.data['temperature'] !== undefined)
        this.temperature_component.set_temperature(this.get_temperature());
      if (v.data['wind_direction'] !== undefined && v.data['wind_speed'] !== undefined) {
        const wind = this.get_wind();
        this.wind_direction_component.set_wind(wind.direction, wind.speed);
      }
    }
  }

  private get_temperature(): number {
    let temp = 0;
    let count = 0;
    for (const sensor of this.env_sensors) {
      const datum = sensor.get_datum();
      if (datum && datum.data['temperature'] !== undefined) {
        temp += datum.data['temperature'] as number;
        count++;
      }
    }
    return count > 0 ? temp / count : 0;
  }

  private get_wind(): { direction: number, speed: number } {
    let dir = 0;
    let speed = 0;
    let count = 0;
    for (const sensor of this.env_sensors) {
      const datum = sensor.get_datum();
      if (datum && datum.data['wind_direction'] !== undefined && datum.data['wind_speed'] !== undefined) {
        dir += datum.data['wind_direction'] as number;
        speed += datum.data['wind_speed'] as number;
        count++;
      }
    }
    if (count > 0) {
      return { direction: dir / count, speed: speed / count };
    } else {
      return { direction: 0, speed: 0 };
    }
  }

  override unmounting(): void {
    super.unmounting();
    for (const sensor of this.env_sensors)
      sensor.remove_item_listener(this);
    coco.CoCo.get_instance().remove_coco_listener(this);
  }
}