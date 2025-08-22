{
  "targets": [
    {
      "target_name": "is_even",
      "sources": [ "src/is_even.cc","src/get_module_filename.cc","src/build_code.cc" ],
      "cflags!": [ "-fno-exceptions" ],
      "cxxflags!": [ "-fno-exceptions" ],
      "conditions": [
        [ "OS=='win'", {
          "msvs_disabled_warnings": [ 4244, 4267 ]
        }]
      ]
    }
  ]
}