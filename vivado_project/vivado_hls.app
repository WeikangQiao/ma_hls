<project xmlns="com.autoesl.autopilot.project" name="vivado_project" top="fpga_top">
    <includePaths/>
    <libraryPaths/>
    <Simulation>
        <SimFlow name="csim" clean="true" csimMode="0" lastCsimMode="0"/>
    </Simulation>
    <files xmlns="">
        <file name="../../networks/AllPoolToSq3x3S2/network.cpp" sc="0" tb="1" cflags=" "/>
        <file name="../../netconfig.cpp" sc="0" tb="1" cflags=" "/>
        <file name="../../cpu_top.cpp" sc="0" tb="1" cflags=" "/>
        <file name="networks/AllPoolToSq3x3S2/network.hpp" sc="0" tb="false" cflags=""/>
        <file name="fpga_top.hpp" sc="0" tb="false" cflags=""/>
        <file name="fpga_top.cpp" sc="0" tb="false" cflags="-I./networks/AllPoolToSq3x3S2 -I."/>
    </files>
    <solutions xmlns="">
        <solution name="solution1" status="active"/>
    </solutions>
</project>

