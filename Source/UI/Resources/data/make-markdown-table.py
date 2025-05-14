import re
from collections import defaultdict

def parse_rdx_file(file_path):
    games = []
    current_game = None
    
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as file:
        for line in file:
            line = line.strip()
            
            # Start of a new game entry
            if line.startswith('[') and ']' in line:
                if current_game:
                    games.append(current_game)
                current_game = {'title': '', 'region': '', 'version': '', 'product_id': ''}
            
            # Parse Good Name
            if current_game and line.startswith('Good Name='):
                current_game['title'] = line[len('Good Name='):].strip()
                
                # Extract region
                region_match = re.search(r'\((E|U|J|G|F|A|I|S|UK)\)', current_game['title'])
                if region_match:
                    region_code = region_match.group(1)
                    regions = {
                        'E': {'name': 'Europe', 'flag': 'eu'},
                        'U': {'name': 'USA', 'flag': 'us'},
                        'J': {'name': 'Japan', 'flag': 'jp'},
                        'G': {'name': 'Germany', 'flag': 'de'},
                        'F': {'name': 'France', 'flag': 'fr'},
                        'A': {'name': 'Australia', 'flag': 'au'},
                        'I': {'name': 'Italy', 'flag': 'it'},
                        'S': {'name': 'Spain', 'flag': 'es'},
                        'UK': {'name': 'United Kingdom', 'flag': 'gb'}
                    }
                    if region_code in regions:
                        current_game['region'] = regions[region_code]
                
                # Extract version
                version_match = re.search(r'\(V(\d+\.\d+)\)', current_game['title'])
                if version_match:
                    current_game['version'] = version_match.group(1)
            
            # Parse Product ID
            if current_game and line.startswith('ProductID='):
                current_game['product_id'] = line[len('ProductID='):].strip()
    
    # Add the last game if there is one
    if current_game:
        games.append(current_game)
    
    return games

def create_region_markdown_table(games, region_info):
    """Create a markdown table for a specific region"""
    flag_code = region_info['flag']
    region_name = region_info['name']
    
    # Create table header with region flag and name
    markdown = f"## <img src='https://cdn.jsdelivr.net/gh/lipis/flag-icons/flags/4x3/{flag_code}.svg' width='30'> {region_name}\n\n"
    
    # Count for progress statistics
    total_games = len(games)
    box_art_completed = 0
    label_art_completed = 0
    
    # Add progress section
    markdown += "Art Type | Completed | Total | Completion Percentage\n"
    markdown += "---------|-----------|-------|-----------------------\n"
    
    # Calculate label art percentage
    label_art_percentage = round(label_art_completed / total_games * 100) if total_games > 0 else 0
    markdown += f"Label Art | {label_art_completed} | {total_games} | <img alt=\"{label_art_percentage}%\" src=\"https://progress-bar.xyz/{label_art_percentage}\">\n"
    
    # Calculate box art percentage
    box_art_percentage = round(box_art_completed / total_games * 100) if total_games > 0 else 0
    markdown += f"Box Art | {box_art_completed} | {total_games} | <img alt=\"{box_art_percentage}%\" src=\"https://progress-bar.xyz/{box_art_percentage}\">\n\n"
    
    # Add table for games
    markdown += "| Title | Version | Product ID | Box Art | Label Art |\n"
    markdown += "|-------|---------|------------|:-------:|:----------:|\n"
    
    for game in games:
        # Escape pipe characters in title
        title = game['title'].replace('|', '\\|')
        
        # Set default emojis for box and label art
        box_art = "❌"
        label_art = "❌"
        
        # Add row to table
        markdown += f"| {title} | {game['version']} | {game['product_id']} | {box_art} | {label_art} |\n"
    
    return markdown

def create_markdown_tables(games):
    # Group games by region
    games_by_region = defaultdict(list)
    region_info = {}
    
    for game in games:
        if game['region']:
            region_code = game['region']['flag']
            games_by_region[region_code].append(game)
            region_info[region_code] = game['region']
    
    # Games with no region information
    unknown_games = [game for game in games if not game['region']]
    
    # Generate markdown for each region
    markdown = "# Nintendo 64 Games Collection\n\n"
    
    # Define region order for consistent output
    region_order = ['us', 'eu', 'jp', 'gb', 'de', 'fr', 'it', 'es', 'au']
    
    # Add tables for each region in order
    for region_code in region_order:
        if region_code in games_by_region and games_by_region[region_code]:
            markdown += create_region_markdown_table(games_by_region[region_code], region_info[region_code])
            markdown += "\n\n"
    
    # Add tables for any other regions not in the predefined order
    for region_code in games_by_region:
        if region_code not in region_order and games_by_region[region_code]:
            markdown += create_region_markdown_table(games_by_region[region_code], region_info[region_code])
            markdown += "\n\n"
    
    # Add table for unknown region games if any
    if unknown_games:
        markdown += "## Unknown Region\n\n"
        
        # Add progress section for unknown region
        total_games = len(unknown_games)
        markdown += "Art Type | Completed | Total | Completion Percentage\n"
        markdown += "---------|-----------|-------|-----------------------\n"
        markdown += f"Label Art | 0 | {total_games} | <img alt=\"0%\" src=\"https://progress-bar.xyz/0\">\n"
        markdown += f"Box Art | 0 | {total_games} | <img alt=\"0%\" src=\"https://progress-bar.xyz/0\">\n\n"
        
        markdown += "| Title | Version | Product ID | Box Art | Label Art |\n"
        markdown += "|-------|---------|------------|:-------:|:----------:|\n"
        
        for game in unknown_games:
            title = game['title'].replace('|', '\\|')
            markdown += f"| {title} | {game['version']} | {game['product_id']} | ❌ | ❌ |\n"
    
    return markdown

def main():
    # Path to the RDX file
    file_path = 'c:\\Users\\iansk\\Desktop\\GitHub\\p64-qt\\UI\\Resources\\data\\Project64.rdx'
    output_file = 'c:\\Users\\iansk\\Desktop\\GitHub\\p64-qt\\n64_games.md'
    
    try:
        games = parse_rdx_file(file_path)
        markdown_tables = create_markdown_tables(games)
        
        with open(output_file, 'w', encoding='utf-8') as file:
            file.write(markdown_tables)
        
        print(f"Successfully created {output_file} with {len(games)} games organized by region.")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == '__main__':
    main()