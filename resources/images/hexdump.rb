
def Dump(name)
    dname = File.basename(name,File.extname(name))
    data = File.open(name,"rb").read

    txt ="\""
    #txt << "const char* resource_#{dname}=\""
    txt << data.unpack("C*").map {|i| "\\x%02x" % i}.join()
    txt << "\",\n"
    size = data.unpack("C*").size

    return {:name=>dname, :data=>txt, :size=>size}
end

h = File.open("wxresource.h","wt")

dumps=[]
Dir["*.png"].each { |pic|
    p pic
    dumps.push(Dump(pic))
}

File.open("wxresource.cpp","wt") { |cpp|
    cpp << "#include \"wxresource.h\"\n\n"

    cpp << "const int resources_sizes[]={"
    dumps.each { |d| cpp << d[:size]<<","}
    cpp << "};\n"

    cpp << "const char* resources[]={\n"
    dumps.each { |d| cpp << d[:data]}
    cpp << "};\n\n"

    cpp << "wxBitmap wxResourceGetBitmap(size_t id)\n"
    cpp <<"{\n"
    cpp <<" wxASSERT(id<#{dumps.size});\n"
    cpp <<" if (id>=#{dumps.size}) return wxNullBitmap;\n"
    cpp <<" wxMemoryInputStream is(resources[id], resources_sizes[id]);\n"
    cpp <<" return wxBitmap(wxImage(is, wxBITMAP_TYPE_ANY, -1), -1); \n"
    cpp <<"}\n"
}

File.open("wxresource.h","wt") { |head|
    head << "#include \"wx/bitmap.h\"\n"
    head << "#include \"wx/mstream.h\"\n\n"
    dumps.each_with_index { |d,i| head << "#define wxRES_#{d[:name]} #{i}\n"}

    head << "wxBitmap wxResourceGetBitmap(size_t id);\n"
}
