import sqlite3
import re
import os
import datetime
from pathlib import Path

class RomDatabaseConverter:
    def __init__(self, db_path="rom_database.sqlite"):
        """Initialize the converter with target database path"""
        self.db_path = db_path
        self.conn = None
        self.cursor = None
    
    def create_database(self):
        """Create a new SQLite database with appropriate schema"""
        self.conn = sqlite3.connect(self.db_path)
        self.cursor = self.conn.cursor()
        
        # Create Regions table
        self.cursor.execute('''
        CREATE TABLE IF NOT EXISTS regions (
            id INTEGER PRIMARY KEY,
            code TEXT UNIQUE,
            name TEXT
        )
        ''')
        
        # Create Developers table
        self.cursor.execute('''
        CREATE TABLE IF NOT EXISTS developers (
            id INTEGER PRIMARY KEY,
            name TEXT UNIQUE
        )
        ''')
        
        # Create Genres table
        self.cursor.execute('''
        CREATE TABLE IF NOT EXISTS genres (
            id INTEGER PRIMARY KEY,
            name TEXT UNIQUE
        )
        ''')
        
        # Create Cartridge Colors table
        self.cursor.execute('''
        CREATE TABLE IF NOT EXISTS cartridge_colors (
            id INTEGER PRIMARY KEY,
            name TEXT UNIQUE
        )
        ''')
        
        # Create Games table with foreign keys
        self.cursor.execute('''
        CREATE TABLE IF NOT EXISTS games (
            id INTEGER PRIMARY KEY,
            rom_id TEXT UNIQUE,
            good_name TEXT,
            internal_name TEXT,
            status TEXT,
            region_id INTEGER,
            cartridge_code TEXT,
            developer_id INTEGER,
            genre_id INTEGER,
            release_date TEXT,
            release_year INTEGER,
            players INTEGER,
            force_feedback INTEGER,
            cartridge_color_id INTEGER,
            FOREIGN KEY (region_id) REFERENCES regions (id),
            FOREIGN KEY (developer_id) REFERENCES developers (id),
            FOREIGN KEY (genre_id) REFERENCES genres (id),
            FOREIGN KEY (cartridge_color_id) REFERENCES cartridge_colors (id)
        )
        ''')
        
        # Create Core Settings table
        self.cursor.execute('''
        CREATE TABLE IF NOT EXISTS core_settings (
            id INTEGER PRIMARY KEY,
            game_id INTEGER,
            setting_name TEXT,
            setting_value TEXT,
            FOREIGN KEY (game_id) REFERENCES games (id),
            UNIQUE(game_id, setting_name)
        )
        ''')
        
        # Create Audio Settings table
        self.cursor.execute('''
        CREATE TABLE IF NOT EXISTS audio_settings (
            id INTEGER PRIMARY KEY,
            game_id INTEGER,
            setting_name TEXT,
            setting_value TEXT,
            FOREIGN KEY (game_id) REFERENCES games (id),
            UNIQUE(game_id, setting_name)
        )
        ''')
        
        # Create Video Settings table
        self.cursor.execute('''
        CREATE TABLE IF NOT EXISTS video_settings (
            id INTEGER PRIMARY KEY,
            game_id INTEGER,
            setting_name TEXT,
            setting_value TEXT,
            FOREIGN KEY (game_id) REFERENCES games (id),
            UNIQUE(game_id, setting_name)
        )
        ''')
        
        # Create Setting Descriptions table
        self.cursor.execute('''
        CREATE TABLE IF NOT EXISTS setting_descriptions (
            id INTEGER PRIMARY KEY,
            category TEXT,
            setting_name TEXT,
            description TEXT,
            possible_values TEXT,
            default_value TEXT,
            UNIQUE(category, setting_name)
        )
        ''')
        
        # Create Metadata table
        self.cursor.execute('''
        CREATE TABLE IF NOT EXISTS metadata (
            id INTEGER PRIMARY KEY,
            key TEXT UNIQUE,
            value TEXT
        )
        ''')
        
        # Initialize regions table with common region codes
        self.initialize_regions()
        
        # Initialize cartridge colors table
        self.initialize_cartridge_colors()
        
        self.conn.commit()
    
    def initialize_regions(self):
        """Initialize the regions table with common region codes"""
        regions = [
            ("4A", "Japan"),
            ("45", "USA"),
            ("50", "Europe"),
            ("55", "Australia"),
            ("44", "Germany"),
            ("46", "France"),
            ("49", "Italy"),
            ("53", "Spain"),
            ("58", "Europe (Multi-language)"),
            ("59", "Europe (Multi-language)"),
            ("41", "International"),
            ("37", "Beta"),
            ("42", "Brazillian"),
            ("54", "Taiwan"),
            ("20", "Development"),
        ]
        
        self.cursor.executemany("""
        INSERT OR IGNORE INTO regions (code, name)
        VALUES (?, ?)
        """, regions)
        self.conn.commit()
    
    def initialize_cartridge_colors(self):
        """Initialize the cartridge colors table with common colors"""
        colors = [
            ("Green",),
            ("Red",),
            ("Yellow",),
            ("Black",),
            ("Gray",),
            ("Gold",),
            ("Gold/Silver",),
        ]
        
        self.cursor.executemany("""
        INSERT OR IGNORE INTO cartridge_colors (name)
        VALUES (?)
        """, colors)
        self.conn.commit()
    
    def get_or_create_region(self, code):
        """Get region ID by code or create if it doesn't exist"""
        if not code:
            return None
            
        self.cursor.execute("SELECT id FROM regions WHERE code = ?", (code,))
        result = self.cursor.fetchone()
        
        if result:
            return result[0]
        
        # If code not found, add as unknown region
        self.cursor.execute("INSERT INTO regions (code, name) VALUES (?, ?)", 
                           (code, f"Unknown Region ({code})"))
        return self.cursor.lastrowid
    
    def get_or_create_developer(self, developer_name):
        """Get developer ID by name or create if it doesn't exist"""
        if not developer_name:
            return None
            
        self.cursor.execute("SELECT id FROM developers WHERE name = ?", (developer_name,))
        result = self.cursor.fetchone()
        
        if result:
            return result[0]
        
        self.cursor.execute("INSERT INTO developers (name) VALUES (?)", (developer_name,))
        return self.cursor.lastrowid
    
    def get_or_create_genre(self, genre_name):
        """Get genre ID by name or create if it doesn't exist"""
        if not genre_name:
            return None
            
        self.cursor.execute("SELECT id FROM genres WHERE name = ?", (genre_name,))
        result = self.cursor.fetchone()
        
        if result:
            return result[0]
        
        self.cursor.execute("INSERT INTO genres (name) VALUES (?)", (genre_name,))
        return self.cursor.lastrowid
    
    def get_cartridge_color_id(self, color_name):
        """Get cartridge color ID by name"""
        if not color_name:
            return None
            
        self.cursor.execute("SELECT id FROM cartridge_colors WHERE name = ?", (color_name,))
        result = self.cursor.fetchone()
        
        if result:
            return result[0]
        
        return None
    
    def extract_rom_details(self, rom_id):
        """Extract cartridge code from ROM ID"""
        cartridge_code = None
        
        if "-C:" in rom_id:
            parts = rom_id.split("-C:")
            if len(parts) >= 2:
                cartridge_code = parts[1]
                
        return cartridge_code
    
    def parse_release_date(self, date_str):
        """Parse release date string into date components
        
        Returns a tuple of (full_date, year) where full_date is the complete date string
        and year is the extracted year as an integer, or None if not valid
        """
        if not date_str:
            return None, None
            
        # Try to extract year with various formats
        year_match = re.search(r'\b(19\d\d|20\d\d)\b', date_str)
        if not year_match:
            return None, None
            
        # Extract year as integer
        year = int(year_match.group(1))
        
        # Try to validate full date
        try:
            # Check if it has standard date format like YYYY-MM-DD or similar
            for fmt in ('%Y-%m-%d', '%Y/%m/%d', '%m/%d/%Y', '%d/%m/%Y', '%B %d, %Y', '%d %B %Y'):
                try:
                    date_obj = datetime.datetime.strptime(date_str, fmt)
                    # If successful, return the original string and the year
                    return date_str, year
                except ValueError:
                    continue
            
            # If no standard format matched, but we have a year, return the original string and year
            return date_str, year
            
        except Exception:
            # If date parsing fails, just return the year if we have it
            return date_str, year
    
    def parse_rdb_file(self, file_path, table_name="core_settings"):
        """Parse a Project64 RDB file and extract data"""
        games = []
        current_game = None
        metadata = {}
        
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as file:
            lines = file.readlines()
        
        i = 0
        while i < len(lines):
            line = lines[i].strip()
            i += 1
            
            # Skip empty lines and comments
            if not line or line.startswith('//'):
                continue
            
            # Check for section headers
            if line.startswith('[') and ']' in line:
                section = line[1:line.find(']')]
                
                if section == "Meta" or section == "Rom Status" or section == "Microcode Identifiers" or section == "ucode":
                    # Handle metadata sections
                    section_key = section
                    while i < len(lines):
                        inner_line = lines[i].strip()
                        i += 1
                        
                        if not inner_line or inner_line.startswith('//'):
                            continue
                        
                        if inner_line.startswith('['):
                            i -= 1  # Go back one line to process next section
                            break
                            
                        if '=' in inner_line:
                            key, value = inner_line.split('=', 1)
                            metadata[f"{section_key}.{key.strip()}"] = value.strip()
                else:
                    # This is a game ROM ID
                    if current_game:
                        games.append(current_game)
                    
                    rom_id = section
                    cartridge_code = self.extract_rom_details(rom_id)
                    
                    current_game = {
                        'rom_id': rom_id,
                        'cartridge_code': cartridge_code,
                        'settings': {},
                        'developer': None,
                        'genre': None,
                        'release_date': None,
                        'release_year': None,
                        'players': None,
                        'force_feedback': None
                    }
            # Process game properties
            elif current_game is not None and '=' in line:
                key, value = line.split('=', 1)
                key = key.strip()
                value = value.strip()
                
                # Handle special keys
                if key == "Good Name":
                    current_game['good_name'] = value
                elif key == "Internal Name":
                    current_game['internal_name'] = value
                elif key == "Status":
                    current_game['status'] = value
                elif key == "ProductID":  # This is the actual cartridge ID (e.g. NUS-NSME-USA)
                    current_game['product_id'] = value
                elif key == "Developer":
                    current_game['developer'] = value
                elif key == "Genre":
                    current_game['genre'] = value
                elif key == "ReleaseDate":
                    full_date, year = self.parse_release_date(value)
                    current_game['release_date'] = full_date
                    current_game['release_year'] = year
                elif key == "Players":
                    try:
                        players = int(value)
                        if 1 <= players <= 4:
                            current_game['players'] = players
                    except ValueError:
                        pass
                elif key == "ForceFeedback" or key == "Rumble":
                    current_game['force_feedback'] = value.lower() in ('yes', 'true', '1')
                else:
                    # Store as a setting
                    current_game['settings'][key] = value
        
        # Don't forget the last game
        if current_game:
            games.append(current_game)
        
        return games, metadata
    
    def parse_rdx_file(self, file_path):
        """Parse a Project64 RDX file to extract additional game metadata"""
        games = []
        current_game = None
        metadata = {}
        
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as file:
            content = file.read()
        
        # Split the file into sections more carefully to preserve CRC IDs
        # First find the header (everything before the first game entry)
        header_end = content.find("\n[")
        if header_end == -1:
            # No games found
            return games, metadata
        
        header = content[:header_end + 1]
        remaining_content = content[header_end + 1:]
        
        # Extract meta section if it exists
        meta_match = re.search(r'\[Meta\](.*?)(?=\n\[|\Z)', content, re.DOTALL)
        if meta_match:
            meta_section = meta_match.group(1).strip()
            for line in meta_section.split('\n'):
                if '=' in line:
                    key, value = line.split('=', 1)
                    metadata[f"Meta.{key.strip()}"] = value.strip()
        
        # Now split the remaining content by game entries
        sections = re.split(r'\n\[', remaining_content)
        
        for i, section in enumerate(sections):
            if not section.strip():
                continue
                
            # Restore the opening bracket that was removed during splitting
            if i > 0:
                section = '[' + section
            
            # Extract the game ID
            id_match = re.match(r'\[([^\]]+)\]', section)
            if not id_match:
                continue
                
            rom_id = id_match.group(1)
            cartridge_code = self.extract_rom_details(rom_id)
            
            # Extract various properties
            good_name_match = re.search(r'Good Name=([^\n]+)', section)
            internal_name_match = re.search(r'Internal Name=([^\n]+)', section)
            status_match = re.search(r'Status=([^\n]+)', section)
            product_id_match = re.search(r'ProductID=([^\n]+)', section)  # This is the actual cartridge code (NUS-XXXX-XXX)
            developer_match = re.search(r'Developer=([^\n]+)', section)
            genre_match = re.search(r'Genre=([^\n]+)', section)
            release_date_match = re.search(r'ReleaseDate=([^\n]+)', section)
            players_match = re.search(r'Players=([1-4])', section)
            force_feedback_match = re.search(r'ForceFeedback=(Yes|No|True|False|1|0)', section, re.IGNORECASE)
            rumble_match = re.search(r'Rumble=(Yes|No|True|False|1|0)', section, re.IGNORECASE)
            
            # Parse release date
            release_date = None
            release_year = None
            if release_date_match:
                release_date, release_year = self.parse_release_date(release_date_match.group(1).strip())
            
            # Parse players count
            players = None
            if players_match:
                try:
                    players = int(players_match.group(1))
                except ValueError:
                    pass
            
            # Parse force feedback (rumble) support
            force_feedback = None
            if force_feedback_match:
                force_feedback = force_feedback_match.group(1).lower() in ('yes', 'true', '1')
            elif rumble_match:
                force_feedback = rumble_match.group(1).lower() in ('yes', 'true', '1')
            
            current_game = {
                'rom_id': rom_id,
                'cartridge_code': cartridge_code,
                'good_name': good_name_match.group(1).strip() if good_name_match else None,
                'internal_name': internal_name_match.group(1).strip() if internal_name_match else None,
                'status': status_match.group(1).strip() if status_match else None,
                'product_id': product_id_match.group(1).strip() if product_id_match else None,
                'developer': developer_match.group(1).strip() if developer_match else None,
                'genre': genre_match.group(1).strip() if genre_match else None,
                'release_date': release_date,
                'release_year': release_year,
                'players': players,
                'force_feedback': force_feedback,
                'settings': {}
            }
            
            # Extract settings
            setting_pattern = r'(?m)^([A-Za-z0-9 ]+)=(.+)$'
            settings_matches = re.finditer(setting_pattern, section)
            for match in settings_matches:
                key = match.group(1).strip()
                value = match.group(2).strip()
                
                # Skip already processed keys
                if key in ["Good Name", "Internal Name", "Status", "ProductID", "Developer", 
                         "Genre", "ReleaseDate", "Players", "ForceFeedback", "Rumble"]:
                    continue
                
                current_game['settings'][key] = value
            
            games.append(current_game)
        
        return games, metadata
    
    def parse_info_file(self, file_path):
        """Parse the RDB Info file to extract setting descriptions"""
        descriptions = []
        
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as file:
            content = file.read()
        
        # Extract category sections
        category_pattern = r'\*\*\*\* ([^\*]+) \*\*\*\*\n(.*?)(?=\*\*\*\*|\Z)'
        categories = re.findall(category_pattern, content, re.DOTALL)
        
        for category_name, category_content in categories:
            category_name = category_name.strip()
            
            # Extract individual settings
            setting_pattern = r'([^\n]+)\n-+\n((?:- [^\n]+\n)*)(.*?)(?=\n[^\n]+\n-+|\Z)'
            settings = re.findall(setting_pattern, category_content, re.DOTALL)
            
            for setting_match in settings:
                if len(setting_match) >= 3:
                    setting_name = setting_match[0].strip()
                    possible_values = setting_match[1].strip()
                    description = setting_match[2].strip()
                    
                    # Extract default value
                    default_value = None
                    default_match = re.search(r'- ([^\n]+) \(default\)', possible_values)
                    if default_match:
                        default_value = default_match.group(1)
                    
                    descriptions.append({
                        'category': category_name,
                        'setting_name': setting_name,
                        'description': description,
                        'possible_values': possible_values,
                        'default_value': default_value
                    })
        
        return descriptions
    
    def import_games(self, games, table_name="core_settings"):
        """Import games into the database"""
        for game in games:
            # Get related entity IDs
            region_id = None
            if 'cartridge_code' in game and game['cartridge_code']:
                cartridge_code = game['cartridge_code']
                region_id = self.get_or_create_region(cartridge_code)
            
            developer_id = self.get_or_create_developer(game.get('developer'))
            genre_id = self.get_or_create_genre(game.get('genre'))
            
            # Check if game already exists
            self.cursor.execute("SELECT id FROM games WHERE rom_id = ?", (game['rom_id'],))
            result = self.cursor.fetchone()
            
            if result:
                game_id = result[0]
                # Update existing game
                self.cursor.execute("""
                UPDATE games 
                SET good_name = COALESCE(?, good_name),
                    internal_name = COALESCE(?, internal_name),
                    status = COALESCE(?, status),
                    region_id = COALESCE(?, region_id),
                    cartridge_code = COALESCE(?, cartridge_code),
                    developer_id = COALESCE(?, developer_id),
                    genre_id = COALESCE(?, genre_id),
                    release_date = COALESCE(?, release_date),
                    release_year = COALESCE(?, release_year),
                    players = COALESCE(?, players),
                    force_feedback = COALESCE(?, force_feedback)
                WHERE id = ?
                """, (
                    game.get('good_name'), 
                    game.get('internal_name'),
                    game.get('status'),
                    region_id,
                    game.get('product_id'),  # Using product_id from RDX file as cartridge_code
                    developer_id,
                    genre_id,
                    game.get('release_date'),
                    game.get('release_year'),
                    game.get('players'),
                    game.get('force_feedback'),
                    game_id
                ))
            else:
                # Insert new game
                self.cursor.execute("""
                INSERT INTO games 
                (rom_id, good_name, internal_name, status, region_id, cartridge_code, 
                developer_id, genre_id, release_date, release_year, players, force_feedback)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                """, (
                    game['rom_id'],
                    game.get('good_name'),
                    game.get('internal_name'),
                    game.get('status'),
                    region_id,
                    game.get('product_id'),  # Using product_id from RDX file as cartridge_code
                    developer_id,
                    genre_id,
                    game.get('release_date'),
                    game.get('release_year'),
                    game.get('players'),
                    game.get('force_feedback')
                ))
                game_id = self.cursor.lastrowid
            
            # Import settings
            for key, value in game.get('settings', {}).items():
                self.cursor.execute(f"""
                INSERT OR REPLACE INTO {table_name} (game_id, setting_name, setting_value)
                VALUES (?, ?, ?)
                """, (game_id, key, value))
    
    def import_metadata(self, metadata):
        """Import metadata into the database"""
        for key, value in metadata.items():
            self.cursor.execute("""
            INSERT OR REPLACE INTO metadata (key, value)
            VALUES (?, ?)
            """, (key, value))
    
    def import_descriptions(self, descriptions):
        """Import setting descriptions into the database"""
        for desc in descriptions:
            self.cursor.execute("""
            INSERT OR REPLACE INTO setting_descriptions 
            (category, setting_name, description, possible_values, default_value)
            VALUES (?, ?, ?, ?, ?)
            """, (
                desc['category'],
                desc['setting_name'],
                desc['description'],
                desc['possible_values'],
                desc['default_value']
            ))
    
    def convert(self, rdb_path, rdx_path=None, audio_rdb_path=None, video_rdb_path=None, info_path=None):
        """Convert RDB files to SQLite database"""
        self.create_database()
        
        # Parse main RDB file
        rdb_games, rdb_metadata = self.parse_rdb_file(rdb_path)
        self.import_games(rdb_games, "core_settings")
        self.import_metadata(rdb_metadata)
        
        # Parse RDX file if provided (contains more detailed metadata)
        if rdx_path and os.path.exists(rdx_path):
            rdx_games, rdx_metadata = self.parse_rdx_file(rdx_path)
            self.import_games(rdx_games, "core_settings")
            self.import_metadata(rdx_metadata)
        
        # Parse audio RDB if provided
        if audio_rdb_path and os.path.exists(audio_rdb_path):
            audio_games, audio_meta = self.parse_rdb_file(audio_rdb_path)
            self.import_games(audio_games, "audio_settings")
            self.import_metadata(audio_meta)
        
        # Parse video RDB if provided
        if video_rdb_path and os.path.exists(video_rdb_path):
            video_games, video_meta = self.parse_rdb_file(video_rdb_path)
            self.import_games(video_games, "video_settings")
            self.import_metadata(video_meta)
        
        # Parse info file if provided
        if info_path and os.path.exists(info_path):
            descriptions = self.parse_info_file(info_path)
            self.import_descriptions(descriptions)
        
        self.conn.commit()
        
        # Print some statistics
        self.cursor.execute("SELECT COUNT(*) FROM games")
        game_count = self.cursor.fetchone()[0]
        
        self.cursor.execute("SELECT COUNT(*) FROM games WHERE cartridge_code IS NOT NULL")
        cartridge_code_count = self.cursor.fetchone()[0]
        
        self.cursor.execute("SELECT COUNT(*) FROM games WHERE players IS NOT NULL")
        players_count = self.cursor.fetchone()[0]
        
        self.cursor.execute("SELECT COUNT(*) FROM games WHERE force_feedback = 1")
        rumble_count = self.cursor.fetchone()[0]
        
        print(f"Database created at {self.db_path} with {game_count} games")
        print(f"- {cartridge_code_count} games have cartridge codes")
        print(f"- {players_count} games have player count information")
        print(f"- {rumble_count} games support force feedback/rumble")
    
    def close(self):
        """Close database connection"""
        if self.conn:
            self.conn.close()


def main():
    # Set paths
    script_dir = os.path.dirname(os.path.abspath(__file__))
    rdb_path = os.path.join(script_dir, "Project64.rdb")
    rdx_path = os.path.join(script_dir, "Project64.rdx")
    audio_rdb_path = os.path.join(script_dir, "Audio.rdb")
    video_rdb_path = os.path.join(script_dir, "Video.rdb")
    info_path = os.path.join(script_dir, "RDB Info.txt")
    db_path = os.path.join(script_dir, "rom_database.sqlite")
    
    # Create and run converter
    converter = RomDatabaseConverter(db_path)
    try:
        converter.convert(rdb_path, rdx_path, audio_rdb_path, video_rdb_path, info_path)
    finally:
        converter.close()


if __name__ == "__main__":
    main()
