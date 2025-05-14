import re
import os

def parse_rdx_file(file_path):
    """Parse the RDX file to extract game entries and their details"""
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as file:
        content = file.read()
    
    # Split the file into sections more carefully to preserve CRC IDs
    # First find the header (everything before the first game entry)
    header_end = content.find("\n[")
    if header_end == -1:
        # No games found
        return content, []
    
    header = content[:header_end + 1]
    remaining_content = content[header_end + 1:]
    
    # Now split the remaining content by game entries
    # Each game entry starts with a line containing just the CRC ID in brackets
    sections = re.split(r'\n\[', remaining_content)
    
    games = []
    for i, section in enumerate(sections):
        if section.strip():  # Skip empty sections
            # Restore the opening bracket that was removed during splitting
            if i > 0:  # The first section doesn't need the bracket restored
                section = '[' + section
            
            # Extract the game ID (CRC ID)
            id_match = re.match(r'([^\]]+)\]', section)
            if id_match:
                game_id = id_match.group(1)
                crc_id = f"[{game_id}]"
                
                # Extract the game title
                title_match = re.search(r'Good Name=([^\n]+)', section)
                title = title_match.group(1) if title_match else ""
                
                # Extract the product ID (if it exists)
                product_match = re.search(r'ProductID=([^\n]+)', section)
                product_id = product_match.group(1) if product_match else ""
                
                # Extract the region from the title
                region_match = re.search(r'\((E|U|J|G|F|A|I|S|UK)\)', title)
                region = region_match.group(1) if region_match else ""
                
                # Extract the cartridge code from the CRC ID
                cartridge_code_match = re.search(r'-C:([0-9A-F]{1,2})(?:\]|$)', crc_id)
                cartridge_code = cartridge_code_match.group(1) if cartridge_code_match else ""
                
                games.append({
                    'id': game_id,
                    'crc_id': crc_id,
                    'title': title,
                    'product_id': product_id,
                    'region': region,
                    'cartridge_code': cartridge_code,
                    'section': section,
                    'section_index': i
                })
    
    return header, games

def is_cartridge_code_valid_for_region(cartridge_code, region, product_id):
    """Check if the cartridge code matches the expected code for the region and product ID suffix is correct"""
    # Common region codes (from the RDX file)
    region_to_cartridge_code = {
        'J': '4A',  # Japan
        'U': '45',  # USA/North America
        'E': '50',  # Europe/PAL
        'G': '44',  # Germany
        'F': '46',  # France
        'A': '55',  # Australia
        'I': '49',  # Italy
        'S': '53',  # Spain
        'UK': '50'  # United Kingdom (using Europe code)
    }
    
    # Region code to expected product ID suffix mapping
    region_to_product_suffix = {
        'J': '-JPN',
        'U': '-USA',
        'E': '-EUR',
        'G': '-DEU',  # Germany
        'F': '-FRA',  # France
        'A': '-AUS',  # Australia
        'I': '-ITA',  # Italy
        'S': '-ESP',  # Spain
        'UK': '-UKV'  # UK
    }
    
    # If no region or cartridge code, we can't validate
    if not region or not cartridge_code or region not in region_to_cartridge_code:
        return True  # Can't validate, assume it's valid
    
    # Check cartridge code
    expected_cart_code = region_to_cartridge_code[region]
    cart_code_valid = cartridge_code == expected_cart_code
    
    # Check product ID suffix if product_id is provided
    suffix_valid = True
    if product_id:
        expected_suffix = region_to_product_suffix.get(region)
        if expected_suffix:
            # Check if the product ID ends with the expected suffix or suffix-1 (for revisions)
            suffix_valid = (product_id.endswith(expected_suffix) or 
                           product_id.endswith(f"{expected_suffix}-1"))
            
            # Special case for some USA product IDs that use -MSA, -ASM, etc.
            if region == 'U' and not suffix_valid:
                usa_alt_suffixes = ['-MSA', '-ASM', '-NOA']
                suffix_valid = any(product_id.endswith(suffix) for suffix in usa_alt_suffixes)
    
    return cart_code_valid and suffix_valid

def parse_codes_file(file_path):
    """Parse the CODES.md file to get game names and product IDs"""
    codes = []
    
    with open(file_path, 'r', encoding='utf-8') as file:
        for line in file:
            line = line.strip()
            
            # Skip comment lines and empty lines
            if not line or line.startswith('<!--'):
                continue
            
            # Look for the last tab or multiple spaces that divide title and product ID
            if '\t' in line:
                parts = line.split('\t')
                title = parts[0].strip()
                product_id = parts[-1].strip()
                codes.append({'title': title, 'product_id': product_id})
            else:
                # Try to match a pattern where the product ID is at the end: NUS-XXXX-XXX
                match = re.search(r'(.*?)\s+(NUS-[A-Z0-9]{3,4}-[A-Z]{3}(?:-\d+)?)$', line)
                if match:
                    title = match.group(1).strip()
                    product_id = match.group(2).strip()
                    codes.append({'title': title, 'product_id': product_id})
    
    return codes

def normalize_title(title):
    """Normalize a title by removing special characters, spaces, and converting to lowercase.
    Only keep alphanumeric characters for comparison."""
    # First, remove region indicators like (U), (E), (J), etc.
    title = re.sub(r'\([A-Z]\)', '', title)
    # Remove version indicators like (V1.0), (V1.1), etc.
    title = re.sub(r'\(V\d+\.\d+\)', '', title)
    # Remove other common indicators
    title = re.sub(r'\[!\]|\(M\d\)|\[f\d\]|\(beta\)|\(demo\)', '', title)
    
    # Remove all characters except alphanumeric
    return ''.join(c for c in title.lower() if c.isalnum())

def match_games(rdx_games, codes_games):
    """Match games between RDX and CODES.md, requiring exact match after normalization"""
    result = {
        'matches': [],
        'updated': [],
        'unchanged': [],
        'not_in_rdx': [],
        'not_in_codes': []
    }
    
    # Create a dictionary for quick lookup of code games by normalized title
    codes_dict = {}
    for code_game in codes_games:
        normalized_title = normalize_title(code_game['title'])
        if normalized_title:  # Only add if not empty after normalization
            codes_dict[normalized_title] = code_game
    
    # Try to match RDX games to CODES.md games
    for rdx_game in rdx_games:
        normalized_title = normalize_title(rdx_game['title'])
        if normalized_title in codes_dict:
            # We have an exact match after normalization
            code_game = codes_dict[normalized_title]
            result['matches'].append({'rdx_game': rdx_game, 'code_game': code_game})
            
            # Check if product ID needs to be updated
            rdx_product_id = rdx_game.get('product_id', '').strip()
            code_product_id = code_game.get('product_id', '').strip()
            
            if code_product_id and code_product_id != rdx_product_id:
                result['updated'].append({'rdx_game': rdx_game, 'code_game': code_game})
            elif rdx_product_id:  # Product ID already exists and matches
                result['unchanged'].append({'rdx_game': rdx_game, 'code_game': code_game})
        else:
            # No match found
            result['not_in_codes'].append(rdx_game)
    
    # Find codes games not in RDX
    rdx_normalized_titles = {normalize_title(game['title']) for game in rdx_games}
    for code_game in codes_games:
        normalized_title = normalize_title(code_game['title'])
        if normalized_title and normalized_title not in rdx_normalized_titles:
            result['not_in_rdx'].append(code_game)
    
    return result

def update_rdx_file(file_path, header, all_games, result):
    """Update the RDX file with new product IDs"""
    # Track what was actually updated
    updates = []
    warnings = []
    
    # Update game sections that need updates
    for update_info in result['updated']:
        rdx_game = update_info['rdx_game']
        code_game = update_info['code_game']
        old_product_id = rdx_game['product_id']
        new_product_id = code_game['product_id']
        
        # Validate cartridge code matches region and product ID has correct suffix
        if 'region' in rdx_game and 'cartridge_code' in rdx_game:
            if not is_cartridge_code_valid_for_region(rdx_game['cartridge_code'], rdx_game['region'], new_product_id):
                warning = f"Warning: Skipped update for {rdx_game['title']}. "
                warning += f"Cartridge code {rdx_game['cartridge_code']} or product ID {new_product_id} "
                warning += f"doesn't match region code {rdx_game['region']}"
                warnings.append(warning)
                print(warning)
                continue  # Skip this update
        
        section = rdx_game['section']
        
        # Check if ProductID line exists
        if 'ProductID=' in section:
            # Update existing ProductID line
            updated_section = re.sub(
                r'ProductID=.*',
                f'ProductID={new_product_id}',
                section
            )
        else:
            # Add ProductID line before the end of the section
            lines = section.split('\n')
            # Find the right position (before the first blank line or end)
            insert_pos = next((i for i, line in enumerate(lines) if not line.strip()), len(lines) - 1)
            lines.insert(insert_pos, f'ProductID={new_product_id}')
            updated_section = '\n'.join(lines)
        
        rdx_game['section'] = updated_section
        
        updates.append({
            'title': rdx_game['title'],
            'old_product_id': old_product_id,
            'new_product_id': new_product_id
        })
    
    # Reconstruct the file content
    games_sorted = sorted(all_games, key=lambda g: g['section_index'])
    
    # Start with the header
    content = header
    
    # Add each game section
    for game in games_sorted:
        if game['section_index'] > 0:  # Skip the first section which is already included in the header
            content += '\n' + game['section']
    
    # Write the updated content back to the file
    with open(file_path, 'w', encoding='utf-8') as file:
        file.write(content)
    
    return updates, warnings

def generate_summary(result, updates, warnings):
    """Generate a summary of changes made"""
    summary = "# Product ID Update Summary\n\n"
    
    # Summary statistics
    summary += f"## Statistics\n\n"
    summary += f"- Total games in RDX: {len(result['matches']) + len(result['not_in_codes'])}\n"
    summary += f"- Total entries in CODES.md: {len(result['matches']) + len(result['not_in_rdx'])}\n"
    summary += f"- Games matched: {len(result['matches'])}\n"
    summary += f"- Games updated: {len(updates)}\n"
    summary += f"- Games unchanged: {len(result['unchanged'])}\n"
    summary += f"- Games in RDX not found in CODES.md: {len(result['not_in_codes'])}\n"
    summary += f"- Codes not matched to any game: {len(result['not_in_rdx'])}\n"
    summary += f"- Updates skipped due to region/cartridge code mismatch: {len(warnings)}\n\n"
    
    # Warnings section
    if warnings:
        summary += "## Warnings - Updates Skipped\n\n"
        for warning in warnings:
            summary += f"- {warning}\n"
        summary += "\n"
    
    # Updated games
    if updates:
        summary += "## Updated Games\n\n"
        summary += "| Title | Old Product ID | New Product ID |\n"
        summary += "|-------|---------------|---------------|\n"
        for update in updates:
            summary += f"| {update['title']} | {update['old_product_id']} | {update['new_product_id']} |\n"
        summary += "\n"
    
    # Unchanged games
    if result['unchanged']:
        summary += "## Unchanged Games\n\n"
        summary += "| Title | Product ID |\n"
        summary += "|-------|------------|\n"
        for game in result['unchanged']:
            summary += f"| {game['rdx_game']['title']} | {game['rdx_game']['product_id']} |\n"
        summary += "\n"
    
    # Games not in CODES.md
    if result['not_in_codes']:
        summary += "## Games Not Found in CODES.md\n\n"
        summary += "| Title |\n"
        summary += "|-------|\n"
        for game in result['not_in_codes']:
            summary += f"| {game['title']} |\n"
        summary += "\n"
    
    # Codes not matched
    if result['not_in_rdx']:
        summary += "## Codes Not Matched to Any Game\n\n"
        summary += "| Title | Product ID |\n"
        summary += "|-------|------------|\n"
        for code in result['not_in_rdx']:
            summary += f"| {code['title']} | {code['product_id']} |\n"
    
    return summary

def main():
    rdx_file_path = 'c:\\Users\\iansk\\Desktop\\GitHub\\p64-qt\\UI\\Resources\\data\\Project64.rdx'
    codes_file_path = 'c:\\Users\\iansk\\Desktop\\GitHub\\p64-qt\\CODES.md'
    summary_file_path = 'c:\\Users\\iansk\\Desktop\\GitHub\\p64-qt\\update_summary.md'
    
    # Create backup of RDX file
    backup_path = rdx_file_path + '.bak'
    with open(rdx_file_path, 'r', encoding='utf-8', errors='ignore') as src:
        with open(backup_path, 'w', encoding='utf-8') as dst:
            dst.write(src.read())
    
    print(f"Created backup of RDX file at {backup_path}")
    
    # Parse files
    header, rdx_games = parse_rdx_file(rdx_file_path)
    codes_games = parse_codes_file(codes_file_path)
    
    print(f"Found {len(rdx_games)} games in RDX file")
    print(f"Found {len(codes_games)} entries in CODES.md")
    
    # Match games and determine updates
    result = match_games(rdx_games, codes_games)
    
    # Update RDX file
    updates, warnings = update_rdx_file(rdx_file_path, header, rdx_games, result)
    
    # Generate and write summary
    summary = generate_summary(result, updates, warnings)
    with open(summary_file_path, 'w', encoding='utf-8') as f:
        f.write(summary)
    
    print(f"Updated {len(updates)} games in RDX file")
    print(f"Generated {len(warnings)} warnings about potential region mismatches")
    print(f"Summary written to {summary_file_path}")

if __name__ == "__main__":
    main()
