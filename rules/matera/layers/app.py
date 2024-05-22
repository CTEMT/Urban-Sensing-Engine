from flask import Flask, render_template

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/Attrezzature_Ludiche_PdV')
def attrezzature_ludiche_pdv():
    return render_template('Attrezzature_Ludiche_PdV.geojson')

@app.route('/chiese_rupestri')
def chiese_rupestri():
    return render_template('Chiese_Rupestri.geojson')

@app.route('/chiese')
def chiese():
    return render_template('Chiese.geojson')

if __name__ == '__main__':
    app.run(debug=True)