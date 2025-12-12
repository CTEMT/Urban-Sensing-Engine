import { Component } from "@ratiosolver/flick";
import * as echarts from 'echarts';
import type { ECharts, EChartsOption } from 'echarts';

export class WindComponent extends Component<HTMLDivElement> {

  private gaugeChart?: ECharts;
  private direction: number = 0;
  private speed: number = 0;

  constructor() {
    super(document.createElement('div'));
    this.node.id = 'wind-component';
    Object.assign(this.node.style, { width: '350px', height: '300px', margin: '1rem auto' });

    this.initializeChartWhenReady();
    window.addEventListener('resize', this.handleResize);
  }

  public set_direction(value: number): void {
    this.direction = value;
    this.updateGauge();
  }

  public set_speed(value: number): void {
    this.speed = value;
    this.updateGauge();
  }

  public set_wind(direction: number, speed: number): void {
    this.direction = direction;
    this.speed = speed;
    this.updateGauge();
  }

  private initializeChartWhenReady(): void {
    if (this.node.isConnected && this.node.clientWidth > 0 && this.node.clientHeight > 0) {
      this.createGauge();
      return;
    }

    requestAnimationFrame(() => this.initializeChartWhenReady());
  }

  private createGauge(): void {
    if (this.gaugeChart) {
      return;
    }

    this.gaugeChart = echarts.init(this.node);
    this.updateGauge();
  }

  private updateGauge(): void {
    if (!this.gaugeChart) {
      return;
    }

    this.gaugeChart.setOption(this.buildOption());
  }

  private buildOption(): EChartsOption {
    return {
      series: [
        {
          type: 'gauge',
          center: ['50%', '60%'],
          startAngle: 90,
          endAngle: -270,
          min: 0,
          max: 360,
          splitNumber: 12,
          anchor: {
            show: false
          },
          title: {
            show: false
          },
          pointer: {
            show: false
          },
          axisLabel: {
            distance: 15,
            color: '#333',
            fontSize: 12,
            formatter: (value: number) => {
              const directions = ['N', 'NE', 'E', 'SE', 'S', 'SW', 'W', 'NW'];
              const index = Math.round(value / 45) % 8;
              return directions[index];
            }
          },
          detail: {
            valueAnimation: true,
            width: '60%',
            lineHeight: 40,
            borderRadius: 8,
            offsetCenter: [0, '-15%'],
            fontSize: 30,
            fontWeight: 'bolder',
            color: 'inherit',
            formatter: (value: number) => value.toFixed(1)
          },
          data: [
            {
              value: this.speed
            }
          ]
        },
        {
          type: 'gauge',
          center: ['50%', '60%'],
          startAngle: 90,
          endAngle: -270,
          min: 0,
          max: 360,
          itemStyle: {
            color: '#FD7347'
          },
          axisLine: {
            show: false
          },
          axisTick: {
            show: false
          },
          splitLine: {
            show: false
          },
          axisLabel: {
            show: false
          },
          detail: {
            show: false
          },
          data: [
            {
              value: this.direction
            }
          ]
        }]
    };
  }

  private handleResize = (): void => {
    this.gaugeChart?.resize();
  };
}