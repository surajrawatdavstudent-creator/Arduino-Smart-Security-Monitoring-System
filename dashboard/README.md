# Excel Dashboard

The Excel dashboard is generated from `data/arduino_data.csv`.

Run:

```text
python python/update_dashboard.py
```

This creates:

`dashboard/Smart_Security_Auto_Dashboard.xlsx`

The dashboard contains:

- Total records
- Granted access count
- Denied access count
- Latest temperature
- Latest humidity
- Latest distance
- Access result chart
- Sensor reading chart

The `.xlsx` file is a generated local file, so it is ignored by Git after it is created. The Python script is the main file that should be kept in the repository.
