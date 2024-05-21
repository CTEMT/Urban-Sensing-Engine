from flask import Flask, render_template

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/chiese_rupestri')
def alberature():
    return render_template('Chiese_Rupestri.geojson')

@app.route('/chiese')
def alberature():
    return render_template('Chiese.geojson')

if __name__ == '__main__':
    app.run(debug=True)