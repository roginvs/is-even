{
  "targets": [
    {
      "target_name": "is_even",
      "sources": [
        "src/is_even.cc",
        "src/module.getFileName.cc",
        "src/codegen.cc",
        "src/module.init.cc"
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