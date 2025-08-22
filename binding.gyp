{
  "targets": [
    {
      "target_name": "is_even",
      "sources": [
        "src/is_even.cc",
        "src/get_module_filename.cc",
        "src/build_code.cc",
        "src/init_module.cc"
      ],
      "cflags": [ "-fno-exceptions", "-Wall", "-Wextra", "-Wunused" ],
      "cxxflags": [ "-fno-exceptions", "-Wall", "-Wextra", "-Wunused" ],
      "conditions": [
        [ "OS=='win'", {
          "msvs_disabled_warnings": [ 4244, 4267 ]
        }]
      ]
    }
  ]
}