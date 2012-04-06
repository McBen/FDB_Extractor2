
def Dump(name)
    dname = File.basename(name,File.extname(name))

    data = File.open(name,"rb").read

    txt =""
    txt << "const char* resource_#{dname}=\""
    txt << data.unpack("C*").map {|i| "\\x%02x" % i}.join()
    txt << "\";\n"
    return txt
end

File.open("resource.c","wt") {|f|
    Dir["*.png"].each { |pic|
        p pic
        f<< Dump(pic)
    }
}