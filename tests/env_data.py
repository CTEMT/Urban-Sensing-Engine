import csv
import os
import time
from datetime import datetime, timedelta
from io import StringIO
from typing import Dict, List, Optional

import requests
import urllib3

urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)


def read_secret(secret_name: str, env_var: Optional[str] = None) -> str:
    # Allow fallback to environment variables for local runs outside Docker.
    if env_var:
        value = os.environ.get(env_var)
        if value:
            return value
    secret_path = f'/run/secrets/{secret_name}'
    try:
        with open(secret_path, 'r', encoding='utf-8') as secret_file:
            return secret_file.read().strip()
    except FileNotFoundError as exc:
        raise RuntimeError(
            f'Missing secret {secret_name}. Set {env_var} or mount /run/secrets/{secret_name}.'
        ) from exc


class EnvDataFetcher:
    def __init__(self, data_url: str, coco_url: str, username: str, password: str, devices: List[Dict[str, str]], window_minutes: int = 15) -> None:
        self.data_url = data_url
        self.coco_url = coco_url
        self.username = username
        self.password = password
        self.devices = devices
        self.window_minutes = window_minutes

    def _fetch_device_id(self, device: Dict[str, str]) -> None:
        if device['id']:
            return
        response = requests.get(
            f'{self.coco_url}/items?type=EnvironmentalSensor&name={device["serial"]}')
        if response.status_code != 200:
            print(
                f'Failed to query device {device["serial"]}: {response.status_code}')
            return
        items = response.json()
        if items:
            device['id'] = items[0]['id']
            print(f'Found ID for device {device["serial"]}: {device["id"]}')
        else:
            print(f'No item found for device {device["serial"]}')

    def _build_query_params(self, device_serial: str) -> Dict[str, str]:
        to_date = datetime.now().strftime('%Y-%m-%d %H:%M')
        tmp = datetime.strptime(to_date, '%Y-%m-%d %H:%M')
        from_date = (tmp - timedelta(minutes=self.window_minutes)
                     ).strftime('%Y-%m-%d %H:%M')
        return {'username': self.username, 'password': self.password, 'from': from_date, 'to': to_date, 'device_serial': device_serial}

    def _fetch_measurements(self, params: Dict[str, str]) -> requests.Response:
        query_string = '&'.join(
            [f'{key}={value}' for key, value in params.items()])
        url_with_params = f'{self.data_url}?{query_string}'
        response = requests.get(url_with_params, verify=False)
        print(response.text)
        return response

    def _process_device(self, device: Dict[str, str]) -> None:
        self._fetch_device_id(device)
        params = self._build_query_params(device['serial'])
        response = self._fetch_measurements(params)
        reader = csv.DictReader(StringIO(response.text))
        for row in reader:
            print(row['Device serial'], row['Date '])
            if row['Date '] > device['date']:
                data = {}
                if 'Temperature 1 (Medium) °C' in row and row['Temperature 1 (Medium) °C'].strip() != '':
                    data['temperature'] = float(
                        row['Temperature 1 (Medium) °C'])
                if 'Humidity 2 (Medium) RH%' in row and row['Humidity 2 (Medium) RH%'].strip() != '':
                    data['humidity'] = float(row['Humidity 2 (Medium) RH%'])
                if 'Wind Direction 4 (Medium) GN' in row and row['Wind Direction 4 (Medium) GN'].strip() != '':
                    data['wind_direction'] = float(
                        row['Wind Direction 4 (Medium) GN'])
                if 'Wind Speed 9 (Medium) m/s' in row and row['Wind Speed 9 (Medium) m/s'].strip() != '':
                    data['wind_speed'] = float(
                        row['Wind Speed 9 (Medium) m/s'])
                if 'CO 34 (Medium) ppm' in row and row['CO 34 (Medium) ppm'].strip() != '':
                    data['co'] = float(row['CO 34 (Medium) ppm'])
                if 'NO 35 (Medium) ppm' in row and row['NO 35 (Medium) ppm'].strip() != '':
                    data['no'] = float(row['NO 35 (Medium) ppm'])
                if 'NO2 37 (Medium) ppm' in row and row['NO2 37 (Medium) ppm'].strip() != '':
                    data['no2'] = float(row['NO2 37 (Medium) ppm'])
                if 'O3 38 (Medium) ppm' in row and row['O3 38 (Medium) ppm'].strip() != '':
                    data['o3'] = float(row['O3 38 (Medium) ppm'])
                if 'SO2 39 (Medium) ppm' in row and row['SO2 39 (Medium) ppm'].strip() != '':
                    data['so2'] = float(row['SO2 39 (Medium) ppm'])
                if 'Auxiliary Measurement 41 (Medium) ppm' in row and row['Auxiliary Measurement 41 (Medium) ppm'].strip() != '':
                    data['voc'] = float(
                        row['Auxiliary Measurement 41 (Medium) ppm'])
                if 'Auxiliary Measurement 91 (Medium) ppm' in row and row['Auxiliary Measurement 91 (Medium) ppm'].strip() != '':
                    data['pm1'] = float(
                        row['Auxiliary Measurement 91 (Medium) ppm'])
                if 'Auxiliary Measurement 141 (Medium) ppm' in row and row['Auxiliary Measurement 141 (Medium) ppm'].strip() != '':
                    data['pm2_5'] = float(
                        row['Auxiliary Measurement 141 (Medium) ppm'])
                if 'Auxiliary Measurement 191 (Medium) ppm' in row and row['Auxiliary Measurement 191 (Medium) ppm'].strip() != '':
                    data['pm10'] = float(
                        row['Auxiliary Measurement 191 (Medium) ppm'])
                if 'Atmospheric pressure 2013 (Medium) hPa' in row and row['Atmospheric pressure 2013 (Medium) hPa'].strip() != '':
                    data['atmospheric_pressure'] = float(
                        row['Atmospheric pressure 2013 (Medium) hPa'])
                response = requests.post(
                    f'{self.coco_url}/data/{device["id"]}', json=data)
                device['date'] = row['Date ']
                print('Updated date to:', device['date'])
        print('-----------------------------------')

    def run(self) -> None:
        # Main entry point so CLI callers can just instantiate and call run.
        while True:
            for device in self.devices:
                self._process_device(device)
            print("Waiting 10 minutes before next fetch..")
            time.sleep(600)


def main() -> None:
    data_url = 'https://backend02.nesasrl.eu/v1/measurement/public'
    coco_url = 'http://localhost:8080'
    devices = [
        dict(serial='19819', date='', id=''),
        dict(serial='19820', date='', id=''),
        dict(serial='19821', date='', id=''),
        dict(serial='19822', date='', id=''),
        dict(serial='19823', date='', id=''),
        dict(serial='19824', date='', id=''),
        dict(serial='19825', date='', id=''),
        dict(serial='19826', date='', id=''),
        dict(serial='19827', date='', id=''),
        dict(serial='19828', date='', id=''),
    ]
    username = read_secret('nesa_username', env_var='NESA_USERNAME')
    password = read_secret('nesa_password', env_var='NESA_PASSWORD')
    fetcher = EnvDataFetcher(
        data_url=data_url,
        coco_url=coco_url,
        username=username,
        password=password,
        devices=devices,
    )
    fetcher.run()


if __name__ == '__main__':
    main()
