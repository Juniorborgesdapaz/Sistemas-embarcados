from flask import Flask, render_template
from flask_socketio import SocketIO
from flask_cors import CORS

app = Flask(__name__)
CORS(app)  # Permitir CORS para acesso da Pico W
socketio = SocketIO(app, cors_allowed_origins="*")

@app.route('/')
def index():
    return render_template('index.html')

# Eventos HTTP para Pico W
@app.route('/CLICK', methods=['GET'])
def click():
    print("Comando: CLICK")
    socketio.emit('led_blue', {'status': 'on'})
    return 'LED azul ligado (CLICK)', 200

@app.route('/SOLTO', methods=['GET'])
def solto():
    print("Comando: SOLTO")
    socketio.emit('led_blue', {'status': 'off'})
    return 'LED azul desligado (SOLTO)', 200

@app.route('/RED_ON', methods=['GET'])
def red_on():
    print("Comando: LED vermelho ON")
    socketio.emit('led_red', {'status': 'on'})
    return 'LED vermelho ligado', 200

@app.route('/RED_OFF', methods=['GET'])
def red_off():
    print("Comando: LED vermelho OFF")
    socketio.emit('led_red', {'status': 'off'})
    return 'LED vermelho desligado', 200

# Evento de botão da interface web
@socketio.on('toggle_led')
def handle_toggle(data):
    cor = data.get('color')
    if cor == 'blue':
        print("Web: alternar LED azul")
        socketio.emit('led_blue', {'status': 'on'})
    elif cor == 'red':
        print("Web: alternar LED vermelho")
        socketio.emit('led_red', {'status': 'on'})

if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=5000)
