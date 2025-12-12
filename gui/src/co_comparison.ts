import { coco } from "@ratiosolver/coco";
import { Component } from "@ratiosolver/flick";
import * as echarts from 'echarts';
import type { ECharts, EChartsOption } from 'echarts';

export class EnvironmentComparisonComponent extends Component<HTMLDivElement> implements coco.CoCoListener, coco.taxonomy.ItemListener {

  private readonly name: string;
  private readonly par: string;
  private coCharts?: ECharts;
  private env_sensors: Set<coco.taxonomy.Item> = new Set();
  private readonly sensor_index = new Map<string, number>();
  private readonly sensor_data: Map<string, [number, number][]> = new Map();

  constructor(name: string, par: string) {
    super(document.createElement('div'));
    this.name = name;
    this.par = par;
    this.node.id = 'co-comparison-component';
    Object.assign(this.node.style, { width: '350px', height: '300px', margin: '1rem auto' });

    this.initializeChartWhenReady();
    window.addEventListener('resize', this.handleResize);

    coco.CoCo.get_instance().add_coco_listener(this);
    for (const item of coco.CoCo.get_instance().get_items())
      this.new_item(item);
  }

  new_type(_: coco.taxonomy.Type): void { }
  new_item(item: coco.taxonomy.Item): void {
    if (Array.from(item.get_types()).some(t => t.get_name() === 'EnvironmentalSensor')) {
      this.env_sensors.add(item);
      this.sensor_index.set(item.get_id(), this.env_sensors.size - 1);
      this.sensor_data.set(item.get_id(), []);
      item.add_item_listener(this);
      coco.CoCo.get_instance().load_data(item);
    }
  }
  types_updated(item: coco.taxonomy.Item): void {
    if (Array.from(item.get_types()).some(t => t.get_name() === 'EnvironmentalSensor')) {
      if (!this.env_sensors.has(item)) {
        this.env_sensors.add(item);
        this.sensor_index.set(item.get_id(), this.env_sensors.size - 1);
        this.sensor_data.set(item.get_id(), []);
        item.add_item_listener(this);
      }
    } else {
      if (this.env_sensors.has(item)) {
        this.env_sensors.delete(item);
        this.sensor_index.clear();
        Array.from(this.env_sensors).forEach((s, index) => this.sensor_index.set(s.get_id(), index));
        this.sensor_data.delete(item.get_id());
        item.remove_item_listener(this);
      }
    }
  }
  properties_updated(_: coco.taxonomy.Item): void { }
  values_updated(item: coco.taxonomy.Item): void {
    if (this.env_sensors.has(item)) {
      const data: [number, number][] = [];
      for (const datum of item.get_data())
        if (datum.data[this.par] !== undefined)
          data.push([datum.timestamp.valueOf(), datum.data[this.par] as number]);
      this.sensor_data.set(item.get_id(), data);
      this.updateChart();
    }
  }
  new_value(item: coco.taxonomy.Item, v: coco.taxonomy.Datum): void {
    if (this.env_sensors.has(item)) {
      this.coCharts?.appendData({
        seriesIndex: this.sensor_index.get(item.get_id())!,
        data: [[v.timestamp.valueOf(), v.data[this.par]]]
      });
    }
  }

  private initializeChartWhenReady(): void {
    if (this.node.isConnected && this.node.clientWidth > 0 && this.node.clientHeight > 0) {
      this.createChart();
      return;
    }

    requestAnimationFrame(() => this.initializeChartWhenReady());
  }

  private createChart(): void {
    if (this.coCharts)
      return;

    this.coCharts = echarts.init(this.node);
    this.updateChart();
  }

  private updateChart(): void {
    if (!this.coCharts)
      return;

    this.coCharts.setOption(this.buildOption());
  }

  private buildOption(): EChartsOption {
    return {
      animation: true,
      legend: { data: Array.from(this.env_sensors).map((s) => s.to_string()) },
      toolbox: { feature: { saveAsImage: {}, dataZoom: {}, restore: {} } },
      grid: { left: '10%', right: '10%', top: 60, bottom: 60 },
      xAxis: { type: 'time' },
      yAxis: { type: 'value', name: this.name },
      series: Array.from(this.env_sensors).map((s) => ({
        name: s.to_string(),
        type: 'line',
        showSymbol: false,
        data: this.sensor_data.get(s.get_id()) || []
      })),
      dataZoom: [{ type: 'slider', xAxisIndex: 0 }, { type: 'inside', xAxisIndex: 0 }]
    };
  }

  private handleResize = (): void => {
    this.coCharts?.resize();
  };
}