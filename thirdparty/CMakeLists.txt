include(CPM)

CPMAddPackage("gh:ashtum/lazycsv#master")
CPMAddPackage("gh:jarro2783/cxxopts@3.2.1")

CPMAddPackage(
        GITHUB_REPOSITORY wolfpld/tracy
        GIT_TAG master
        OPTIONS "TRACY_ENABLE OFF"
#        "TRACY_ON_DEMAND ON"
#        "TRACY_NO_BROADCAST ON"
#        "TRACY_NO_SYSTEM_TRACING ON"
#        "TRACY_TIMER_QPC ON"
)

set_target_properties(
        lazycsv
        cxxopts
        PROPERTIES FOLDER "thirdparty")