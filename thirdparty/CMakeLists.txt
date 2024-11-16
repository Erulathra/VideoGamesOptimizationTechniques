include(CPM)

CPMAddPackage("gh:ashtum/lazycsv#master")
CPMAddPackage("gh:jarro2783/cxxopts@3.2.1")

set_target_properties(
        lazycsv
        cxxopts
        PROPERTIES FOLDER "thirdparty")