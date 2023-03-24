require "yaml"
scan_codes = File.read("../../res/NuPhy/keycodes.yml")
scancode_by_key = YAML.load(scan_codes)
key_by_scancode = scancode_by_key.invert

map = File.read(ARGV[0])
for line in map.split("\n")
    line.chomp!
    offset, b0, b1, b2, b3 = line.split(/\s+/)
    scan_code = (b3.to_i(16) << 24) | (b2.to_i(16) << 16) | (b1.to_i(16) << 8) | b0.to_i(16);
    if key_by_scancode[scan_code].nil?
        puts line
    else
        puts "#{line} -> #{key_by_scancode[scan_code]}"
    end
end