from flask import Flask, request, jsonify
from flask_cors import CORS
import psycopg2
from psycopg2.extras import RealDictCursor
import os
from dotenv import load_dotenv

load_dotenv()

app = Flask(__name__)
CORS(app)

DB_HOST = os.getenv('DB_HOST', 'localhost')
DB_PORT = os.getenv('DB_PORT', '5432')
DB_USER = os.getenv('DB_USER', 'alek')
DB_PASSWORD = os.getenv('DB_PASSWORD', '123456')
DB_NAME = os.getenv('DB_NAME', 'fitness_app')
FLASK_ENV = os.getenv('FLASK_ENV', 'development')
FLASK_DEBUG = os.getenv('FLASK_DEBUG', 'True') == 'True'
API_PORT = int(os.getenv('API_PORT', 5000))

print(f"[CONFIG] DB_HOST: {DB_HOST}")
print(f"[CONFIG] DB_NAME: {DB_NAME}")
print(f"[CONFIG] DB_USER: {DB_USER}")
print(f"[CONFIG] FLASK_ENV: {FLASK_ENV}")

def get_db_connection():
    try:
        conn = psycopg2.connect(
            host=DB_HOST,
            port=DB_PORT,
            database=DB_NAME,
            user=DB_USER,
            password=DB_PASSWORD,
            connect_timeout=10
        )
        return conn
    except Exception as e:
        print(f"[ERROR] Database connection: {e}")
        raise

def init_db():
    try:
        conn = get_db_connection()
        cur = conn.cursor()
        
        cur.execute('''
            CREATE TABLE IF NOT EXISTS workouts (
                id SERIAL PRIMARY KEY,
                exercise_name VARCHAR(100) NOT NULL,
                duration_minutes INT NOT NULL,
                calories_burned INT NOT NULL,
                date TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        ''')
        
        conn.commit()
        cur.close()
        conn.close()
        print("[SUCCESS] Database initialized successfully")
    except Exception as e:
        print(f"[WARNING] Database already initialized or error: {e}")

try:
    init_db()
except Exception as e:
    print(f"[WARNING] Could not initialize database: {e}")

@app.route('/api/workouts', methods=['GET'])
def get_workouts():
    try:
        conn = get_db_connection()
        cur = conn.cursor(cursor_factory=RealDictCursor)
        cur.execute('SELECT * FROM workouts ORDER BY date DESC')
        workouts = cur.fetchall()
        cur.close()
        conn.close()
        return jsonify(workouts), 200
    except Exception as e:
        print(f"[ERROR] GET /api/workouts: {str(e)}")
        import traceback
        traceback.print_exc()
        return jsonify({'error': str(e)}), 500

@app.route('/api/workouts', methods=['POST'])
def add_workout():
    try:
        data = request.json
        
        if not data or 'exercise_name' not in data or 'duration_minutes' not in data or 'calories_burned' not in data:
            return jsonify({'error': 'Missing required fields'}), 400
        
        conn = get_db_connection()
        cur = conn.cursor()
        
        cur.execute('''
            INSERT INTO workouts (exercise_name, duration_minutes, calories_burned)
            VALUES (%s, %s, %s)
            RETURNING id, exercise_name, duration_minutes, calories_burned, date
        ''', (data['exercise_name'], data['duration_minutes'], data['calories_burned']))
        
        workout = cur.fetchone()
        conn.commit()
        cur.close()
        conn.close()
        
        return jsonify({
            'id': workout[0],
            'exercise_name': workout[1],
            'duration_minutes': workout[2],
            'calories_burned': workout[3],
            'date': str(workout[4])
        }), 201
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/workouts/<int:id>', methods=['DELETE'])
def delete_workout(id):
    try:
        conn = get_db_connection()
        cur = conn.cursor()
        
        cur.execute('DELETE FROM workouts WHERE id = %s', (id,))
        conn.commit()
        cur.close()
        conn.close()
        
        return jsonify({'message': 'Workout deleted'}), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/stats', methods=['GET'])
def get_stats():
    try:
        conn = get_db_connection()
        cur = conn.cursor(cursor_factory=RealDictCursor)
        
        cur.execute('''
            SELECT 
                COUNT(*) as total_workouts,
                COALESCE(SUM(duration_minutes), 0) as total_minutes,
                COALESCE(SUM(calories_burned), 0) as total_calories,
                COALESCE(AVG(calories_burned), 0) as avg_calories
            FROM workouts
        ''')
        
        stats = cur.fetchone()
        cur.close()
        conn.close()
        
        return jsonify(stats), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/health', methods=['GET'])
def health():
    try:
        conn = get_db_connection()
        conn.close()
        return jsonify({'status': 'healthy', 'database': 'connected'}), 200
    except Exception as e:
        return jsonify({'status': 'unhealthy', 'database': 'disconnected'}), 500

if __name__ == '__main__':
    app.run(
        host='0.0.0.0',
        port=API_PORT,
        debug=FLASK_DEBUG,
        threaded=True
    )