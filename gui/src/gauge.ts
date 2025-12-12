import { Component } from '@ratiosolver/flick';
import * as echarts from 'echarts';
import type { ECharts, EChartsOption } from 'echarts';

export class GaugeComponent extends Component<HTMLDivElement> {

  private gaugeChart?: ECharts;
  private value = 12;
  private readonly min: number;
  private readonly max: number;
  private readonly split_number;
  private readonly formatter: (value: number) => string;

  constructor(id: string, min: number, max: number, formatter: (value: number) => string, split_number: number = 12) {
    super(document.createElement('div'));
    this.node.id = id;
    this.min = min;
    this.max = max;
    this.split_number = split_number;
    this.formatter = formatter;
    Object.assign(this.node.style, { width: '350px', height: '300px', margin: '1rem auto' });

    this.initializeChartWhenReady();
    window.addEventListener('resize', this.handleResize);
  }

  public set_value(value: number): void {
    this.value = value;
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
      series: [{
        type: 'gauge',
        center: ['50%', '60%'],
        startAngle: 200,
        endAngle: -20,
        min: this.min,
        max: this.max,
        splitNumber: this.split_number,
        itemStyle: {
          color: '#FFAB91'
        },
        progress: {
          show: true,
          width: 30
        },
        pointer: {
          show: false
        },
        axisLine: {
          lineStyle: {
            width: 30
          }
        },
        axisTick: {
          distance: -45,
          splitNumber: 5,
          lineStyle: {
            width: 2,
            color: '#999'
          }
        },
        splitLine: {
          distance: -52,
          length: 14,
          lineStyle: {
            width: 3,
            color: '#999'
          }
        },
        axisLabel: {
          distance: -20,
          color: '#999',
          fontSize: 20
        },
        anchor: {
          show: false
        },
        title: {
          show: false
        },
        detail: {
          valueAnimation: true,
          width: '60%',
          lineHeight: 40,
          borderRadius: 8,
          offsetCenter: [0, '-15%'],
          fontSize: 30,
          fontWeight: 'bolder',
          formatter: this.formatter,
          color: 'inherit'
        },
        data: [
          {
            value: this.value
          }
        ]
      },
      {
        type: 'gauge',
        center: ['50%', '60%'],
        startAngle: 200,
        endAngle: -20,
        min: this.min,
        max: this.max,
        itemStyle: {
          color: '#FD7347'
        },
        progress: {
          show: true,
          width: 8
        },
        pointer: {
          show: false
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
            value: this.value
          }
        ]
      }]
    };
  }

  private handleResize = (): void => {
    this.gaugeChart?.resize();
  };
}