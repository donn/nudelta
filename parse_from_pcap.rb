#!/usr/bin/env ruby
if ARGV.count != 3
    STDERR.puts "Usage: #{$0} <.pcapng file> <usb address> <to_folder>"
    exit 64
end
pcap_file = ARGV[0]
usb_address = ARGV[1]
to_folder = ARGV[2]

# ---

$line_rx = /(\d+)\s+((?:[A-Za-z0-9]{2} ?)+)/

class Integer
    def to_hex
        "%02x" % self
    end
end

class Array
    def hex_pretty_print
        self.map { |el| el.to_hex }.each_slice(8).map { |sl| sl.join(" ") }.join("\n    ")
    end

    def to_hex
        self.pack("c*").unpack("H*").first
    end

    def to_i32
        self.pack("c*").unpack("L").first
    end

    def to_i64
        self.pack("c*").unpack("Q").first
    end
end

class Urb
    attr_accessor :raw, :setup_data_raw
    attr_accessor :id
    attr_accessor :urb_type, :urb_transfer_type
    attr_accessor :endpoint
    attr_accessor :device
    attr_accessor :bus_id
    attr_accessor :setup_req
    attr_accessor :data_status

    attr_accessor :sec, :usec

    attr_accessor :urb_status, :data_status, :urb_length, :data_length

    attr_accessor :request_type, :report_id, :report_type

    attr_accessor :interval, :frame, :transfer_flag, :iso_desc_count

    attr_accessor :data, :reply

    def initialize(bytes)
        @raw = bytes
        @setup_data_raw = bytes[40..47].to_hex

        #---
        @id = bytes[0..7].to_hex

        @urb_type = nil
        case bytes[8]
        when 0x43
            @urb_type = "complete"
        when 0x53
            @urb_type = "submit"
        else
            @urb_type = "unknown:#{bytes[8].to_hex}"
        end
        @urb_transfer_type = bytes[9].to_hex

        @endpoint = bytes[10].to_hex
        @device = bytes[11].to_hex
        @bus_id = bytes[12..13].to_hex
        @setup_req = bytes[14]
        @data_status = bytes[15]

        @sec = bytes[16..23].to_i64
        @usec = bytes[24..27].to_hex

        @urb_status = bytes[28..31].to_hex
        @urb_length = bytes[32..35].to_i32
        @data_length = bytes[36..39].to_i32

        @request_type = nil
        case bytes[41]
        when 0x09
            @request_type = "Set Report"
        when 0x01
            @request_type = "Get Report"
        else
            @request_type = "unknown:#{bytes[41].to_hex}"
        end

        @report_id = bytes[42].to_hex
        @report_type = bytes[43].to_hex

        
        @interval = bytes[48..51].to_i32
        @frame = bytes[52..55].to_i32

        @transfer_flags = bytes[56..59].to_hex
        @iso_desc_count = bytes[60..63].to_i32

        @data = bytes[64..]

        @reply = nil
    end

    def add_reply(reply)
        if @id != reply.id
            raise "mismatched id"
        end
        @reply = reply
    end

    def to_s
        base = <<~HD
        URB Transaction (ID: #{@id})
        #{@request_type} #{@report_id}
        Data Sent (#{@data_length} bytes):
            #{@data.hex_pretty_print}
        HD
        base.chomp!

        unless @reply.nil?
            base += "\nData Recieved (#{@reply.data.count} bytes)\n    #{@reply.data.hex_pretty_print}"
        else
            base += "\nNo reply found!\n"
        end


        return <<~HD
        #<
        #{base}
        >
        HD
    end
end

$section_rx = /^([^\(]+) \(\d+ bytes\):\s*$/
def frame_only(raw)
    final_lines = []
    adding = true
    for line in raw.split("\n")
        match = $section_rx.match (line)
        unless match.nil?
            if match[1].downcase == "frame"
                adding = true
            else
                adding = false
            end
            next
        end 
        if adding
            final_lines << line
        end
    end
    return final_lines.join("\n")
end

$hex_line_rx = /(\d+)\s+((?:[A-Za-z0-9]{2} ?)+)/
def hex_dump_to_bytes(hex_dump)
    all_bytes = []
    for line in hex_dump.split("\n")
        match = $hex_line_rx.match(line)
        if match.nil?
            next
        end
        bytes = match[2]
        all_bytes += bytes.split(" ").map{ |b| b.to_i(16) }
    end
    return all_bytes
end

system "rm -rf '#{to_folder}'"
system "mkdir -p '#{to_folder}'"

result = `tshark -x -r '#{pcap_file}' -Y 'usb.addr == "#{usb_address}" and usb.bInterfaceClass == 0x03'`
File.open("#{to_folder}/raw", "w") { |f|
    f.puts result
}
raw_packets = result.split("\n\n")
transaction_list = []
transactions = {}
last_request_id = nil
for packet in raw_packets
    packet = frame_only(packet)
    bytes = hex_dump_to_bytes(packet)
    tx = Urb.new bytes
    
    if transactions[tx.id].nil?
        if tx.urb_type == "submit"
            transactions[tx.id] = tx
            transaction_list.append(tx)
            last_request_id = tx.id
        else
            STDERR.puts "No request found for #{tx.id}. Dropping packet."
        end
    else
        request = transactions[tx.id]
        if tx.urb_type == "submit"
            if last_request_id == tx.id
                STDERR.puts "Dropping duplicate request for #{tx.id}."
            else
                transactions[tx.id] = tx
                transaction_list.append(tx)
                last_request_id = tx.id
            end
        else
            if request.reply.nil?
                request.add_reply tx
            else
                STDERR.puts "Dropping duplicate reply for #{tx.id}."
            end
        end
    end
end

tx_count = 0
for tx in transaction_list
    File.open("#{to_folder}/tx#{tx_count}.tx", "w") { |f|
        f.puts tx
    }
    tx_count += 1
end