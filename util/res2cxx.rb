require "yaml"

resource_dir = ARGV[0]

puts '#include "common.hpp"'
puts '#include "nuphy.hpp"'
for rsc in Dir.glob("#{resource_dir}/**/*.yml")
  directory = File.dirname rsc
  keyboard = File.basename directory
  
  object = YAML.load_file rsc
  
  _, type, name = File.read(rsc).split("\n")[0].split()
  
  if type == "list"
    puts "const std::vector<std::uint32_t> #{keyboard}::#{name} = {"
    for integer in object
      puts "    0x#{format("%08x", integer)},"
    end
    puts "};"
  elsif type == "dict"
    puts "const std::unordered_map<std::string, std::uint32_t> #{keyboard}::#{name} = {"
    object.each do |key, integer|
      puts "    { \"#{key}\", 0x#{format("%08x", integer)} },"
    end
    puts "};"
  end
end
