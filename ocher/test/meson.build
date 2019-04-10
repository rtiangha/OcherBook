testjson = executable('testjson', ['JsonTest.cpp'],
    dependencies : [
        catch_dep,
        json_dep,
    ],
)
test('nlohmann json', testjson)


testsettings = executable('testsettings', ['SettingsTest.cpp'],
    dependencies : [
        catch_dep,
        ocherbook_dep,
    ],
)
test('settings', testsettings)


testocher = executable('testocher', ['OcherTest.cpp'],
    dependencies : [
        catch_dep,
        ocherbook_dep,
    ],
)
test('misc ocher tests', testocher)
