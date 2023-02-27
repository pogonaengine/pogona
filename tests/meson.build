tests = [
  ['vector', ['vector.c']],
]

foreach test_def : tests
  exe = executable(
    test_def[0],
    test_def[1],
    dependencies : libpogona_dep,
  )
  test(test_def[0], exe)
endforeach
