<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:msxsl="urn:schemas-microsoft-com:xslt" 
    xmlns:rd="urn:schemas-microsoft-com:xml-data"    
    xmlns:maksym="urn:maksym-scripts"
    exclude-result-prefixes="msxsl rd maksym">
  
  <xsl:output method="html" indent="yes"/>
  
  <rd:HeaderList>
    <Sources>
      <Source Name="Generic Library" />
      <Source Name="System Library" />
      <Source Name="Application" />
      <Source Name="Unknown" />
      <Source Name="Total" />
    </Sources>
    <Memories>
      <Memory Name="RAM" />
      <Memory Name="ROM" />
      <Memory Name="Total" />
    </Memories>
  </rd:HeaderList>  

  <msxsl:script implements-prefix="maksym" language="C#">
    UInt32 _nextAddress = 0;
    UInt32 _startAddress = 0;
    UInt32 _size = 0;
    UInt32 _position = 0;
    String _bgColor = "#000000";
    String _ax = "";
    
    public String GetAx()
    {
      return _ax;
    }
    
    public void SetAx(String value)
    {
      _ax = value;
    }
    
    public void AddAx(String value)
    {
      _ax += value;
    }
    
    public UInt32 GetNextAddress()
    {
      return _nextAddress;
    }
    
    public void SetNextAddress(UInt32 address)
    {
      _nextAddress = address;
    }
    
    public UInt32 GetStartAddress()
    {
      return _startAddress;
    }
    
    public void SetStartAddress(UInt32 address)
    {
      _startAddress = address;
    }
    
    public void SetSize(UInt32 value)
    {
      _size = value;
    }
    
    public void AddSize(UInt32 value)
    {
      _size += value;  
    }
    
    public void SubSize(UInt32 value)
    {
      _size -= value;
    }
    
    public UInt32 GetSize()
    {
      return _size;
    }
    
    public UInt32 GetPosition()
    {
      return _position;
    }
    
    public void SetPosition(UInt32 position)
    {
      _position = position;
    }
    
    public String GetBgColor()
    {
      return _bgColor;
    }
    
    public void SetBgColor(String bgColor)
    {
      _bgColor = bgColor;
    }
    
    public UInt32 HexToDec(String number)
    {
      return System.Convert.ToUInt32(number, 16);
    }

    public String DecToHex(UInt32 number)
    {
      return "0x" + number.ToString("X8");
    }
  </msxsl:script>
  
  <!-- 
    XSLT Elements
    - xsl:stylesheet - the root element that declares the document to be an XSL style sheet (xsl:transform is synonymous)
    - xsl:template - is used to build templates
                   - the match attribute is used to associate a template with an XML element
                   - the value of the match attribute is an XPath expression (i.e. match="/" defines the whole document)
    - xsl:value-of - is used to extract the value of an XML element and add it to the output stream of the transformation
                   - the select attribute contains an XPath expression
                   - a forward slash (/) selects subdirectories
    - xsl:for-each - is used to select every XML element of a specified node-set
    - xsl:sort - is used to sort the output, simply add xsl:sort inside xsl:for-each
    - xsl:if - is used to put a conditional test against the content of the XML file, e.g.: xsl:if test="price &gt; 10"
    - xsl:choose - is used in conjunction with xsl:when and xsl:otherwise to express multiple conditional tests
    - xsl:apply-templates - applies a template to the current element or to the current element's child nodes
    - xsl:call-template - calls a named template
    - xsl:text - is used to write literal text to the output
    - xsl:attribute - replaces existing attributes with equivalent names
    - xsl:variable - is used to declare a local or global variable
                   - the variable is global if it is declared as a top-level element, and local if it is declared within a template
                   - once a variable is set, it can't be changed or modified
                   - add a value to a variable is possible by xsl:variable or by the select attribute
  -->
  <xsl:template match="Metric">
    <html>
      <head>
        <style type="text/css">
          <xsl:text>
            * {
              margin: auto;
              width: 1000px;
              box-sizing: border-box;
              text-align: center;
              font-size: 11px;
              font-family: Verdana;
            }

            h1 {
              font-size: 14px;
              margin: 20px auto;
            }

            h2 {
              font-size: 12px;
              margin: 20px auto;
            }

            ul.level-0 {
            }

            ul.level-1 {
            }

            li {
              text-align: left;
            }

            table.table-1 {
              border-spacing: 0;
              border-top: 1px solid #58A0DC;
              border-left: 1px solid #58A0DC;
              border-right: none;
              border-bottom: none;
              margin: auto;
            }

            thead.table-1 th {
              border-right: 1px solid #58A0DC;
              border-bottom: 1px solid #58A0DC;
            }

            thead.table-1 tr:first-child {
              background: #B4CFEC;
            }

            thead.table-1 tr:nth-child(2) {
              background: #B4CFEC;
            }

            tbody.table-1 td:first-child {
              white-space: nowrap;
              text-align: left;
              font-weight: bold;
            }

            tbody.table-1 td {
              border-right: 1px solid #58A0DC;
              border-bottom: 1px solid #58A0DC;
              text-align: right;
            }

            table.table-2 {
              width: 800px;
              border-spacing: 0;
              border: 1px solid #58A0DC;
              margin: 20px auto;
            }

            thead.table-2 th {
              border-bottom: 1px solid #58A0DC;
            }

            thead.table-2 tr:first-child {
              background: #B4CFEC;
              border: none;
            }

            table.table-3 {
              width: 800px;
              border-spacing: 0;
              border-top: 1px solid #58A0DC;
              border-left: 1px solid #58A0DC;
              border-right: none;
              border-bottom: none;
              margin: 20px auto;
            }

            thead.table-3 th {
              border-right: 1px solid #58A0DC;
              border-bottom: 1px solid #58A0DC;
            }

            thead.table-3 tr:first-child {
              background: #B4CFEC;
            }

            thead.table-3 tr:nth-child(2) {
              background: #B4CFEC;
            }

            tbody.table-3 td {
              text-align: right;
              font-size: 10px;
              border-right: 1px solid #58A0DC;
            }

            tbody.table-3 td:first-child {

              text-align: center;
            }

            tbody.table-3 td:last-child {
              border-right: 1px solid #58A0DC;
            }

            tbody.table-3 td:nth-child(2) {
              white-space: nowrap;
              text-align: left;
            }

            tbody.table-3 tr:last-child td {
              border-bottom: 1px solid #58A0DC;
            }

            table.table-4 {
              width: 1000px;
              border-spacing: 0;
              border-collapse: collapse;
              border: none;
              margin: 20px auto;
              padding: 0;
            }

            tbody.table-4 td {
              vertical-align: top;
            }

            table.table-5 {
              width: 1000px;
              border-spacing: 0;
              border-collapse: collapse;
              border-top: 1px solid #58A0DC;
              border-left: 1px solid #58A0DC;
              border-right: none;
              border-bottom: none;
              padding: 0;
            }

            thead.table-5 th {
              border-right: 1px solid #58A0DC;
              border-bottom: 1px solid #58A0DC;
            }

            thead.table-5 tr:first-child {
              background: #B4CFEC;
            }

            thead.table-5 tr:nth-child(2) {
              background: #B4CFEC;
            }

            tbody.table-5 td:first-child {
              white-space: nowrap;
              text-align: left;
              font-weight: bold;
            }

            tbody.table-5 td {
              border-right: 1px solid #58A0DC;
              border-bottom: 1px solid #58A0DC;
              text-align: right;
            }

            table.table-6 {
              width: 500px;
              border-spacing: 0;
              border-collapse: collapse;
              border-top: 1px solid #58A0DC;
              border-left: 1px solid #58A0DC;
              border-right: none;
              border-bottom: none;
              padding: 0;
            }

            thead.table-6 tr:first-child {
              background: #D2E2F4;
            }

            thead.table-6 tr:nth-child(2) {
              background: #D2E2F4;
            }

            thead.table-6 th {
              border-right: 1px solid #58A0DC;
              border-bottom: 1px solid #58A0DC;              
            }

            tbody.table-6 td {
              font-size: 10px;
              border-right: 1px solid #58A0DC;
            }

            tbody.table-6 td:first-child {
              white-space: nowrap;
              text-align: left;
            }

            tbody.table-6 td:last-child {
              border-right: 1px solid #58A0DC;
            }

            tbody.table-6 td:nth-child(2) {
              text-align: right;
            }

            tbody.table-6 td:nth-child(3) {
              text-align: right;
            }

            tbody.table-6 tr:last-child {
              border-bottom: 1px solid #58A0DC;
            }

            .unknownSymbol {
              color: #FF0000;
            }

            table.legend {
              width: 800px;
              margin: 0 auto;
            }
  
            tbody.legend td {
              text-align: left;
              font-size: 10px;
            }

            div {
              text-align: center;
            }
        
          </xsl:text>
        </style>
      </head>
      <body>
        <xsl:call-template name="title" />
        <xsl:call-template name="contents" />        
        <xsl:call-template name="total-memory-usage" />
        <xsl:call-template name="section-list" />
        <xsl:call-template name="symbol-section-mapping" />
        <xsl:call-template name="module-list" />
        <xsl:call-template name="symbol-module-mapping" />
      </body>
    </html>
  </xsl:template>
  
  <xsl:template name="title">
    <h1>
      <xsl:text>Memory Usage Report</xsl:text>
    </h1>
    <h2>
      <xsl:text>Source: </xsl:text>
      <xsl:value-of select="@Origin"/>
    </h2>
    <h2>
      <xsl:text>Format: </xsl:text>
      <xsl:value-of select="@FileFormat" />
    </h2>
    <h2>
      <xsl:text>Architecture: </xsl:text>
      <xsl:value-of select="@Architecture" />
    </h2>
    <h2>
      <xsl:text>Generator: </xsl:text>      
      <xsl:value-of select="@Generator"/>
      <xsl:text> </xsl:text>
      <xsl:value-of select="@Version"/>  
    </h2>
  </xsl:template>
  
  <xsl:template name="contents">
    <h2>
      Contents
    </h2>
    <div>
      <ul class="level-0">
        <li>
          <a>
            <xsl:attribute name="href">
              <xsl:value-of select="concat('#', 'total-memory-usage')" />
            </xsl:attribute>
            Total Memory Usage
          </a>
        </li>
        <li>
          <a>
            <xsl:attribute name="href">
              <xsl:value-of select="concat('#', 'section-list')" />
            </xsl:attribute>
            Memory Sections
          </a>
        </li>
        <li>
          <a>
            <xsl:attribute name="href">
              <xsl:value-of select="concat('#', 'symbol-section-mapping')" />
            </xsl:attribute>
            Symbol to Section Mapping
          </a>
          <ul class="level-1">
            <xsl:for-each select="Sections/Section">
              <xsl:sort select="@Location" />
              <xsl:variable name="secStart" select="maksym:HexToDec(@Location)" />
              <xsl:variable name="secSize" select="maksym:HexToDec(@Size)" />
              <xsl:variable name="secEnd" select="$secStart + ($secSize - 1)" />
              <li>
                <a>
                  <xsl:attribute name="href">
                    <xsl:value-of select="concat('#', @Location)"/>
                  </xsl:attribute>
                  <xsl:choose>
                    <xsl:when test="$secEnd &lt; 0">
                      <xsl:value-of select="concat(maksym:DecToHex($secStart), ' - ', maksym:DecToHex('0'))" />
                    </xsl:when>
                    <xsl:otherwise>
                      <xsl:value-of select="concat(maksym:DecToHex($secStart), ' - ', maksym:DecToHex($secEnd))" />
                    </xsl:otherwise>
                  </xsl:choose>
                </a>
              </li>
            </xsl:for-each>
          </ul>
        </li>
        <li>
          <a>
            <xsl:attribute name="href">
              <xsl:value-of select="concat('#', 'module-list')" />
            </xsl:attribute>
            Modules
          </a>
        </li>
        <li>
          <a>
            <xsl:attribute name="href">
              <xsl:value-of select="concat('#', 'symbol-module-mapping')" />
            </xsl:attribute>
            Symbol in Module Mapping
          </a>
          <ul class="level-1">
            <xsl:for-each select="CompilationUnit[(@CUSource = 'Generic' or @CUSource = 'System' or @CUSource = 'Application') and (Function or Variable)]">
              <xsl:sort select="concat(@Path, '\', @Name)" />
              <li>
                <a>
                  <xsl:attribute name="href">
                    <xsl:value-of select="concat('#', @Path, '\', @Name)"/>
                  </xsl:attribute>
                  <xsl:value-of select="concat(@Path, '\', @Name)" />
                </a>
              </li>
            </xsl:for-each>
          </ul>
        </li>
      </ul>
    </div>
  </xsl:template>    

  <xsl:template name="total-memory-usage">
    <h2>
      <xsl:attribute name="id">
        <xsl:value-of select="'total-memory-usage'"/>
      </xsl:attribute>
      Total Memory Usage
    </h2>
    <xsl:call-template name="total-memory" />
  </xsl:template>
  
  <!-- Returns the size of symbols only for specified source of compilation units. -->
  <xsl:template name="calculate-size">
    <xsl:param name="cu-source" />
    <xsl:param name="symbol" />
    <xsl:param name="memory" />
    <xsl:value-of select="maksym:SetPosition(0)" />
    <xsl:value-of select="maksym:SetSize(0)" />
    <xsl:value-of select="maksym:SetStartAddress(0)" />
    <xsl:for-each select="//CompilationUnit[@CUSource = $cu-source]/*[name() = $symbol][@Memory = $memory]">
      <xsl:sort select="maksym:HexToDec(@Location)" data-type="number" order="ascending" />
      <xsl:variable name="sym-start" select="maksym:HexToDec(@Location)" />
      <xsl:variable name="sym-size" select="maksym:HexToDec(@Size)" />
      <xsl:choose>
        <xsl:when test="maksym:GetPosition() &gt; 0">
          <xsl:if test="$sym-start &gt; maksym:GetStartAddress()">
            <xsl:value-of select="maksym:AddSize(maksym:HexToDec(@Size))" />
          </xsl:if>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="maksym:AddSize(maksym:HexToDec(@Size))" />
        </xsl:otherwise>
      </xsl:choose>
      <xsl:value-of select="maksym:SetPosition(maksym:GetPosition() + 1)" />
      <xsl:value-of select="maksym:SetStartAddress($sym-start)" />
    </xsl:for-each>
    <xsl:value-of select="maksym:GetSize()" />
  </xsl:template>

  <!-- Returns the size of the memory which is not possible to specify (variable, function, compilatio unit) .-->
  <xsl:template name="calculate-unknown-size">
    <xsl:param name="section-condition" />
    <xsl:variable name="section-size">
      <xsl:value-of select="maksym:SetSize(0)" />
      <xsl:for-each select="$section-condition">
        <xsl:value-of select="maksym:AddSize(maksym:HexToDec(@Size))" />
      </xsl:for-each>
      <xsl:value-of select="maksym:GetSize()" />
    </xsl:variable>
    <xsl:value-of select="maksym:SetPosition(0)" />
    <xsl:value-of select="maksym:SetSize($section-size)"/>
    <xsl:value-of select="maksym:SetStartAddress(0)" />
    <xsl:for-each select="$section-condition">
      <xsl:sort select="maksym:HexToDec(@Location)" data-type="number" order="ascending" />
      <xsl:variable name="sec-start" select="maksym:HexToDec(@Location)" />
      <xsl:variable name="sec-size" select="maksym:HexToDec(@Size)" />
      <xsl:variable name="sec-end" select="$sec-start + ($sec-size - 1)" />
      <xsl:for-each select="//CompilationUnit[@CUSource != 'Unknown']/*[self::Function | self::Variable]">
        <xsl:sort select="maksym:HexToDec(@Location)" data-type="number" order="ascending" />
        <xsl:variable name="sym-start" select="maksym:HexToDec(@Location)" />
        <xsl:variable name="sym-size" select="maksym:HexToDec(@Size)" />
        <xsl:if test="($sym-start &gt;= $sec-start) and ($sym-start &lt;= $sec-end)">
          <xsl:choose>
            <xsl:when test="maksym:GetPosition() &gt; 0">
              <xsl:if test="$sym-start &gt; maksym:GetStartAddress()">
                <xsl:value-of select="maksym:SubSize($sym-size)" />
              </xsl:if>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="maksym:SubSize($sym-size)" />
            </xsl:otherwise>
          </xsl:choose>
        </xsl:if>
        <xsl:value-of select="maksym:SetPosition(maksym:GetPosition() + 1)" />
        <xsl:value-of select="maksym:SetStartAddress($sym-start)" />
      </xsl:for-each>
    </xsl:for-each>
    <xsl:value-of select="maksym:GetSize()" />
  </xsl:template>

  <xsl:template name="total-memory">
    
    <xsl:variable name="generic-ram-variable">
      <xsl:call-template name="calculate-size">
        <xsl:with-param name="cu-source" select="'Generic'" />
        <xsl:with-param name="symbol" select="'Variable'" />
        <xsl:with-param name="memory" select="'RAM'" />
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="generic-rom-variable">
      <xsl:call-template name="calculate-size">
        <xsl:with-param name="cu-source" select="'Generic'" />
        <xsl:with-param name="symbol" select="'Variable'" />
        <xsl:with-param name="memory" select="'ROM'" />
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="generic-total-variable" select="$generic-ram-variable + $generic-rom-variable" />
    
    <xsl:variable name="generic-ram-function">
      <xsl:call-template name="calculate-size">
        <xsl:with-param name="cu-source" select="'Generic'" />
        <xsl:with-param name="symbol" select="'Function'" />
        <xsl:with-param name="memory" select="'RAM'" />
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="generic-rom-function">
      <xsl:call-template name="calculate-size">
        <xsl:with-param name="cu-source" select="'Generic'" />
        <xsl:with-param name="symbol" select="'Function'" />
        <xsl:with-param name="memory" select="'ROM'" />
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="generic-total-function" select="$generic-ram-function + $generic-rom-function" />
    
    <xsl:variable name="generic-ram-unknown" select="0" />
    <xsl:variable name="generic-rom-unknown" select="0" />
    <xsl:variable name="generic-total-unknown" select="$generic-ram-unknown + $generic-rom-unknown" />
    
    <xsl:variable name="generic-ram-total" select="$generic-ram-variable + $generic-ram-function + $generic-ram-unknown" />
    <xsl:variable name="generic-rom-total" select="$generic-rom-variable + $generic-rom-function + $generic-rom-unknown" />
    <xsl:variable name="generic-total-total" select="$generic-total-variable + $generic-total-function + $generic-total-unknown" />
    
    <xsl:variable name="system-ram-variable">
      <xsl:call-template name="calculate-size">
        <xsl:with-param name="cu-source" select="'System'" />
        <xsl:with-param name="symbol" select="'Variable'" />
        <xsl:with-param name="memory" select="'RAM'" />
      </xsl:call-template>      
    </xsl:variable>
    <xsl:variable name="system-rom-variable">
      <xsl:call-template name="calculate-size">
        <xsl:with-param name="cu-source" select="'System'" />
        <xsl:with-param name="symbol" select="'Variable'" />
        <xsl:with-param name="memory" select="'ROM'" />
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="system-total-variable" select="$system-ram-variable + $system-rom-variable" />
    
    <xsl:variable name="system-ram-function">
      <xsl:call-template name="calculate-size">
        <xsl:with-param name="cu-source" select="'System'" />
        <xsl:with-param name="symbol" select="'Function'" />
        <xsl:with-param name="memory" select="'RAM'" />
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="system-rom-function">
      <xsl:call-template name="calculate-size">
        <xsl:with-param name="cu-source" select="'System'" />
        <xsl:with-param name="symbol" select="'Function'" />
        <xsl:with-param name="memory" select="'ROM'" />
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="system-total-function" select="$system-ram-function + $system-rom-function" />
    
    <xsl:variable name="system-ram-unknown" select="0" />
    <xsl:variable name="system-rom-unknown" select="0" />
    <xsl:variable name="system-total-unknown" select="$system-ram-unknown + $system-rom-unknown" />
    
    <xsl:variable name="system-ram-total" select="$system-ram-variable + $system-ram-function + $system-ram-unknown" />
    <xsl:variable name="system-rom-total" select="$system-rom-variable + $system-rom-function + $system-rom-unknown" />
    <xsl:variable name="system-total-total" select="$system-total-variable + $system-total-function + $system-total-unknown" /> 
    
    <xsl:variable name="application-ram-variable">
      <xsl:call-template name="calculate-size">
        <xsl:with-param name="cu-source" select="'Application'" />
        <xsl:with-param name="symbol" select="'Variable'" />
        <xsl:with-param name="memory" select="'RAM'" />
      </xsl:call-template>      
    </xsl:variable>
    <xsl:variable name="application-rom-variable">
      <xsl:call-template name="calculate-size">
        <xsl:with-param name="cu-source" select="'Application'" />
        <xsl:with-param name="symbol" select="'Variable'" />
        <xsl:with-param name="memory" select="'ROM'" />
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="application-total-variable" select="$application-ram-variable + $application-rom-variable" />
    
    <xsl:variable name="application-ram-function">
      <xsl:call-template name="calculate-size">
        <xsl:with-param name="cu-source" select="'Application'" />
        <xsl:with-param name="symbol" select="'Function'" />
        <xsl:with-param name="memory" select="'RAM'" />
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="application-rom-function">
      <xsl:call-template name="calculate-size">
        <xsl:with-param name="cu-source" select="'Application'" />
        <xsl:with-param name="symbol" select="'Function'" />
        <xsl:with-param name="memory" select="'ROM'" />
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="application-total-function" select="$application-ram-function + $application-rom-function" />
    
    <xsl:variable name="application-ram-unknown" select="0" />
    <xsl:variable name="application-rom-unknown" select="0" />
    <xsl:variable name="application-total-unknown" select="$application-ram-unknown + $application-rom-unknown" />
    
    <xsl:variable name="application-ram-total" select="$application-ram-variable + $application-ram-function + $application-ram-unknown" />
    <xsl:variable name="application-rom-total" select="$application-rom-variable + $application-rom-function + $application-rom-unknown" />
    <xsl:variable name="application-total-total" select="$application-total-variable + $application-total-function + $application-total-unknown" />
    
    <xsl:variable name="unknown-ram-unknown">
      <xsl:call-template name="calculate-unknown-size">
        <xsl:with-param name="section-condition" select="//Section[(@Flag = 'WRITE_ALLOC')]" />
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="unknown-ram-total">
      <xsl:value-of select="$unknown-ram-unknown" />
    </xsl:variable>
    <xsl:variable name="unknown-rom-unknown">
      <xsl:call-template name="calculate-unknown-size">
        <xsl:with-param name="section-condition" select="//Section[(@Flag = 'ALLOC' or @Flag = 'ALLOC_EXECINSTR')]" />
      </xsl:call-template>        
    </xsl:variable>
    <xsl:variable name="unknown-rom-total">
      <xsl:value-of select="$unknown-rom-unknown" />
    </xsl:variable>
    <xsl:variable name="unknown-total-unknown">
      <xsl:value-of select="$unknown-ram-unknown + $unknown-rom-unknown" />
    </xsl:variable>
    <xsl:variable name="unknown-total-total">
      <xsl:value-of select="$unknown-ram-total + $unknown-rom-total" />
    </xsl:variable>

    <xsl:variable name="unknown-ram-variable">
      <xsl:choose>
        <xsl:when test="$unknown-ram-unknown = 0">
          <xsl:value-of select="0" />
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="0" /> <!-- ? -->
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="unknown-rom-variable">
      <xsl:choose>
        <xsl:when test="$unknown-rom-unknown = 0">
          <xsl:value-of select="0" />
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="0" /> <!-- ? -->
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="unknown-total-variable">
      <xsl:choose>
        <xsl:when test="$unknown-ram-variable = '?' or $unknown-rom-variable = '?'">
          <xsl:value-of select="'?'"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$unknown-ram-variable + $unknown-rom-variable" />
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <xsl:variable name="unknown-ram-function">
      <xsl:choose>
        <xsl:when test="$unknown-ram-unknown = 0">
          <xsl:value-of select="0" />
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="0" /> <!-- ? -->
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="unknown-rom-function">
      <xsl:choose>
        <xsl:when test="$unknown-rom-unknown = 0">
          <xsl:value-of select="0" />
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="0" /> <!-- ? -->
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="unknown-total-function">
      <xsl:choose>
        <xsl:when test="$unknown-ram-function = '?' or $unknown-rom-function = '?'">
          <xsl:value-of select="'?'" />
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$unknown-ram-function + $unknown-rom-function" />
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <xsl:variable name="total-ram-variable">
      <xsl:choose>
        <xsl:when test="$unknown-ram-unknown &gt; 0">
          <xsl:value-of select="$generic-ram-variable + $system-ram-variable + $application-ram-variable" /> <!-- ? -->
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$generic-ram-variable + $system-ram-variable + $application-ram-variable" />
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="total-rom-variable">
      <xsl:choose>
        <xsl:when test="$unknown-rom-unknown &gt; 0">
          <xsl:value-of select="$generic-rom-variable + $system-rom-variable + $application-rom-variable" /> <!-- ? -->
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$generic-rom-variable + $system-rom-variable + $application-rom-variable" />
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="total-total-variable">
      <xsl:choose>
        <xsl:when test="($total-ram-variable = '?') or ($total-rom-variable = '?')">
          <xsl:value-of select="'?'" />
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$total-ram-variable + $total-rom-variable" />
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>    
    
    <xsl:variable name="total-ram-function">
      <xsl:choose>
        <xsl:when test="$unknown-ram-unknown &gt; 0">
          <xsl:value-of select="$generic-ram-function + $system-ram-function + $application-ram-function" /> <!-- ? -->
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$generic-ram-function + $system-ram-function + $application-ram-function" />
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="total-rom-function">
      <xsl:choose>
        <xsl:when test="$unknown-rom-unknown &gt; 0">
          <xsl:value-of select="$generic-rom-function + $system-rom-function + $application-rom-function" /> <!-- ? -->
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$generic-rom-function + $system-rom-function + $application-rom-function" />
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="total-total-function">
      <xsl:choose>
        <xsl:when test="($total-ram-function = '?') or ($total-rom-function = '?')">
          <xsl:value-of select="'?'" />
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$total-ram-function + $total-rom-function" />
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    
    <xsl:variable name="total-ram-unknown" select="$unknown-ram-unknown" />
    <xsl:variable name="total-rom-unknown" select="$unknown-rom-unknown" />
    <xsl:variable name="total-total-unknown" select="$unknown-ram-unknown + $unknown-rom-unknown" />
    
    <xsl:variable name="total-ram-total">
      <xsl:value-of select="$generic-ram-total + $system-ram-total + $application-ram-total + $unknown-ram-total" />
    </xsl:variable>
    <xsl:variable name="total-rom-total">
      <xsl:value-of select="$generic-rom-total + $system-rom-total + $application-rom-total + $unknown-rom-total" />
    </xsl:variable>
    <xsl:variable name="total-total-total">
      <xsl:value-of select="$total-ram-total + $total-rom-total" />
    </xsl:variable>
    
    <table class="table-1">
      <thead class="table-1">
        <tr>
          <th>
            <xsl:text></xsl:text>
          </th>
          <xsl:for-each select="document('')/*/rd:HeaderList/Sources/Source">
            <th colspan="3">
              <xsl:value-of select="@Name" />
            </th>
          </xsl:for-each>
        </tr>
        <tr>
          <th>
            <xsl:text></xsl:text>
          </th>
          <xsl:for-each select="document('')/*/rd:HeaderList/Sources/Source">
            <xsl:for-each select="document('')/*/rd:HeaderList/Memories/Memory">
              <th>
                <xsl:value-of select="@Name" />
              </th>
            </xsl:for-each>
          </xsl:for-each>
        </tr>
      </thead>
      <tbody class="table-1">
        <tr>
          <td>
            <xsl:text>Variables</xsl:text>
          </td>

          <!-- Generic Library -->
          <td>
            <xsl:value-of select="$generic-ram-variable" />
          </td>
          <td>
            <xsl:value-of select="$generic-rom-variable" />
          </td>
          <td>
            <xsl:value-of select="$generic-total-variable" />
          </td>

          <!-- System Library -->
          <td>
            <xsl:value-of select="$system-ram-variable" />
          </td>
          <td>
            <xsl:value-of select="$system-rom-variable" />
          </td>
          <td>
            <xsl:value-of select="$system-total-variable" />
          </td>

          <!-- Application -->
          <td>
            <xsl:value-of select="$application-ram-variable" />
          </td>
          <td>
            <xsl:value-of select="$application-rom-variable" />
          </td>
          <td>
            <xsl:value-of select="$application-total-variable" />
          </td>

          <!-- Unknown -->
          <td>
            <xsl:value-of select="$unknown-ram-variable" />
          </td>
          <td>
            <xsl:value-of select="$unknown-rom-variable" />
          </td>
          <td>
            <xsl:value-of select="$unknown-total-variable" />
          </td>

          <!-- Total -->
          <td>
            <xsl:value-of select="$total-ram-variable" />
          </td>
          <td>
            <xsl:value-of select="$total-rom-variable" />
          </td>
          <td>
            <xsl:value-of select="$total-total-variable" />
          </td>
        </tr>
        <tr>
          <td>
            <xsl:text>Functions</xsl:text>
          </td>

          <!-- Generic Library -->
          <td>
            <xsl:value-of select="$generic-ram-function" />
          </td>
          <td>
            <xsl:value-of select="$generic-rom-function" />
          </td>
          <td>
            <xsl:value-of select="$generic-total-function" />
          </td>

          <!-- System Library -->
          <td>
            <xsl:value-of select="$system-ram-function" />
          </td>
          <td>
            <xsl:value-of select="$system-rom-function" />
          </td>
          <td>
            <xsl:value-of select="$system-total-function" />
          </td>

          <!-- Application -->
          <td>
            <xsl:value-of select="$application-ram-function" />
          </td>
          <td>
            <xsl:value-of select="$application-rom-function" />
          </td>
          <td>
            <xsl:value-of select="$application-total-function" />
          </td>

          <!-- Unknown -->
          <td>
            <xsl:value-of select="$unknown-ram-function" />
          </td>
          <td>
            <xsl:value-of select="$unknown-rom-function" />
          </td>
          <td>
            <xsl:value-of select="$unknown-total-function" />
          </td>

          <!-- Total -->
          <td>
            <xsl:value-of select="$total-ram-function" />
          </td>
          <td>
            <xsl:value-of select="$total-rom-function" />
          </td>
          <td>
            <xsl:value-of select="$total-total-function" />
          </td>
        </tr>
        <tr>
          <td>
            <xsl:text>Unknown</xsl:text>
          </td>

          <!-- Generic Library -->
          <td>
            <xsl:value-of select="$generic-ram-unknown" />
          </td>
          <td>
            <xsl:value-of select="$generic-rom-unknown" />
          </td>
          <td>
            <xsl:value-of select="$generic-total-unknown" />
          </td>

          <!-- System Library -->
          <td>
            <xsl:value-of select="$system-ram-unknown" />
          </td>
          <td>
            <xsl:value-of select="$system-rom-unknown" />
          </td>
          <td>
            <xsl:value-of select="$system-total-unknown" />
          </td>

          <!-- Application -->
          <td>
            <xsl:value-of select="$application-ram-unknown" />
          </td>
          <td>
            <xsl:value-of select="$application-rom-unknown" />
          </td>
          <td>
            <xsl:value-of select="$application-total-unknown" />
          </td>

          <!-- Unknown -->
          <td>
            <xsl:value-of select="$unknown-ram-unknown" />
          </td>
          <td>
            <xsl:value-of select="$unknown-rom-unknown" />
          </td>
          <td>
            <xsl:value-of select="$unknown-total-unknown" />
          </td>

          <!-- Total -->
          <td>
            <xsl:value-of select="$total-ram-unknown" />
          </td>
          <td>
            <xsl:value-of select="$total-rom-unknown" />
          </td>
          <td>
            <xsl:value-of select="$total-total-unknown" />
          </td>
        </tr>
        <tr>
          <td>
            <xsl:text>Total Size</xsl:text>
          </td>

          <!-- Generic Library -->
          <td>
            <xsl:value-of select="$generic-ram-total" />
          </td>
          <td>
            <xsl:value-of select="$generic-rom-total" />
          </td>
          <td>
            <xsl:value-of select="$generic-total-total" />
          </td>

          <!-- System Library -->
          <td>
            <xsl:value-of select="$system-ram-total" />
          </td>
          <td>
            <xsl:value-of select="$system-rom-total" />
          </td>
          <td>
            <xsl:value-of select="$system-total-total" />
          </td>

          <!-- Application -->
          <td>
            <xsl:value-of select="$application-ram-total" />
          </td>
          <td>
            <xsl:value-of select="$application-rom-total" />
          </td>
          <td>
            <xsl:value-of select="$application-total-total" />
          </td>

          <!-- Unknown -->
          <td>
            <xsl:value-of select="$unknown-ram-total" />
          </td>
          <td>
            <xsl:value-of select="$unknown-rom-total" />
          </td>
          <td>
            <xsl:value-of select="$unknown-total-total" />
          </td>

          <!-- Total -->
          <td>
            <xsl:value-of select="$total-ram-total" />
          </td>
          <td>
            <xsl:value-of select="$total-rom-total" />
          </td>
          <td>
            <xsl:value-of select="$total-total-total" />
          </td>
        </tr>
      </tbody>
    </table>
  
  </xsl:template>
  
  <xsl:template match="/Metric/Size">
    
    <xsl:variable name="generic-total-variable" select="maksym:HexToDec(@GenericRamVariable) + maksym:HexToDec(@GenericRomVariable)" />
    <xsl:variable name="system-total-variable" select="maksym:HexToDec(@SystemRamVariable) + maksym:HexToDec(@SystemRomVariable)" />    
    <xsl:variable name="application-total-variable" select="maksym:HexToDec(@ApplicationRamVariable) + maksym:HexToDec(@ApplicationRomVariable)" />
    <xsl:variable name="unknown-ram-variable" select="0" />
    <xsl:variable name="unknown-rom-variable" select="0" />
    <xsl:variable name="unknown-total-variable" select="0" />
    <xsl:variable name="total-ram-variable" select="maksym:HexToDec(@GenericRamVariable) + maksym:HexToDec(@SystemRamVariable) + maksym:HexToDec(@ApplicationRamVariable)" />
    <xsl:variable name="total-rom-variable" select="maksym:HexToDec(@GenericRomVariable) + maksym:HexToDec(@SystemRomVariable) + maksym:HexToDec(@ApplicationRomVariable)" />      
    <xsl:variable name="total-total-variable" select="$total-ram-variable + $total-rom-variable" />
    
    <xsl:variable name="generic-total-function" select="maksym:HexToDec(@GenericRamFunction) + maksym:HexToDec(@GenericRomFunction)" />
    <xsl:variable name="system-total-function" select="maksym:HexToDec(@SystemRamFunction) + maksym:HexToDec(@SystemRomFunction)" />
    <xsl:variable name="application-total-function" select="maksym:HexToDec(@ApplicationRamFunction) + maksym:HexToDec(@ApplicationRomFunction)" />
    <xsl:variable name="unknown-ram-function" select="0" />
    <xsl:variable name="unknown-rom-function" select="0" />
    <xsl:variable name="unknown-total-function" select="0" />
    <xsl:variable name="total-ram-function" select="maksym:HexToDec(@GenericRamFunction) + maksym:HexToDec(@SystemRamFunction) + maksym:HexToDec(@ApplicationRamFunction)" />    
    <xsl:variable name="total-rom-function" select="maksym:HexToDec(@GenericRomFunction) + maksym:HexToDec(@SystemRomFunction) + maksym:HexToDec(@ApplicationRomFunction)" />
    <xsl:variable name="total-total-function" select="$total-ram-function + $total-rom-function" />

    <xsl:variable name="generic-ram-unknown" select="0" />
    <xsl:variable name="generic-rom-unknown" select="0" />
    <xsl:variable name="generic-total-unknown" select="0" />
    <xsl:variable name="system-ram-unknown" select="0" />
    <xsl:variable name="system-rom-unknown" select="0" />
    <xsl:variable name="system-total-unknown" select="0" />
    <xsl:variable name="application-ram-unknown" select="0" />
    <xsl:variable name="application-rom-unknown" select="0" />
    <xsl:variable name="application-total-unknown" select="0" />
    
    <xsl:variable name="generic-ram-total" select="maksym:HexToDec(@GenericRamVariable) + maksym:HexToDec(@GenericRamFunction)" />
    <xsl:variable name="generic-rom-total" select="maksym:HexToDec(@GenericRomVariable) + maksym:HexToDec(@GenericRomFunction)" />
    <xsl:variable name="generic-total-total" select="$generic-total-variable + $generic-total-function" />
    <xsl:variable name="system-ram-total" select="maksym:HexToDec(@SystemRamVariable) + maksym:HexToDec(@SystemRamFunction)" />
    <xsl:variable name="system-rom-total" select="maksym:HexToDec(@SystemRomVariable) + maksym:HexToDec(@SystemRomFunction)" />
    <xsl:variable name="system-total-total" select="$system-total-variable + $system-total-function" />
    <xsl:variable name="application-ram-total" select="maksym:HexToDec(@ApplicationRamVariable) + maksym:HexToDec(@ApplicationRamFunction)" />
    <xsl:variable name="application-rom-total" select="maksym:HexToDec(@ApplicationRomVariable) + maksym:HexToDec(@ApplicationRomFunction)" />
    <xsl:variable name="application-total-total" select="$application-ram-total + $application-rom-total" />
    
   <!-- <xsl:variable name="total-ram-unknown" select="0" /> -->
    <xsl:variable name="section-ram">
      <xsl:value-of select="maksym:SetSize(0)" />
      <xsl:for-each select="//Section[(@Flag = 'WRITE_ALLOC') ]">
        <xsl:value-of select="maksym:AddSize(maksym:HexToDec(@Size))" />
      </xsl:for-each>
      <xsl:value-of select="maksym:GetSize()" />
    </xsl:variable>
    
    <xsl:variable name="unknown-ram-unknown">
      <xsl:value-of select="maksym:SetSize($section-ram)"/>
      <xsl:for-each select="//Section[(@Flag = 'WRITE_ALLOC') ]">
        <xsl:sort select="@Location" />
        
        <xsl:variable name="secStart" select="maksym:HexToDec(@Location)" />
        <xsl:variable name="secSize" select="maksym:HexToDec(@Size)" />
        <xsl:variable name="secEnd" select="$secStart + ($secSize - 1)" />        
        
        <xsl:for-each select="//Function | //Variable">
          <xsl:sort select="@Location" />

          <xsl:variable name="symStart" select="maksym:HexToDec(@Location)" />
          <xsl:variable name="symSize" select="maksym:HexToDec(@Size)" />

          <xsl:if test="$symStart &gt;= $secStart and $symStart &lt;= $secEnd">
            <xsl:value-of select="maksym:SubSize(maksym:HexToDec(@Size))" />
          </xsl:if>
 
        </xsl:for-each>
      </xsl:for-each>
      <xsl:value-of select="maksym:GetSize()" />
    </xsl:variable>    
    
    <xsl:variable name="section-rom">
      <xsl:value-of select="maksym:SetSize(0)" />
      <xsl:for-each select="//Section[(@FLAG = 'ALLOC' or @Flag = 'ALLOC_EXECINSTR')]">
        <xsl:value-of select="maksym:AddSize(maksym:HexToDec(@Size))" />
      </xsl:for-each>
      <xsl:value-of select="maksym:GetSize()" />
    </xsl:variable>
    
    <xsl:variable name="unknown-rom-unknown">
      <xsl:value-of select="maksym:SetSize($section-rom)"/>
      <xsl:for-each select="//Section[(@FLAG = 'ALLOC' or @Flag = 'ALLOC_EXECINSTR')]">
        <xsl:sort select="@Location" />
        
        <xsl:variable name="secStart" select="maksym:HexToDec(@Location)" />
        <xsl:variable name="secSize" select="maksym:HexToDec(@Size)" />
        <xsl:variable name="secEnd" select="$secStart + ($secSize - 1)" />

        <xsl:value-of select="maksym:SetNextAddress(0)" />
        
        <xsl:for-each select="//Function | //Variable">
          <xsl:sort select="@Location" />

          <xsl:variable name="symStart" select="maksym:HexToDec(@Location)" />
          <xsl:variable name="symSize" select="maksym:HexToDec(@Size)" />
          
          <xsl:if test="$symStart &gt; maksym:GetNextAddress()">
            <xsl:if test="$symStart &gt;= $secStart and $symStart &lt;= $secEnd">
              <xsl:value-of select="maksym:SubSize(maksym:HexToDec(@Size))" />
            </xsl:if>
          </xsl:if>

          <xsl:value-of select="maksym:SetNextAddress($symStart)" />
    
        </xsl:for-each>
      </xsl:for-each>
      <xsl:value-of select="maksym:GetSize()" />
    </xsl:variable>        

    <!-- <xsl:variable name="unknown-rom-unknown" select="$total-rom-total - $generic-rom-total - $system-rom-total - $application-rom-total" /> -->
    <xsl:variable name="unknown-total-unknown" select="$unknown-ram-unknown + $unknown-rom-unknown" />
    <xsl:variable name="unknown-ram-total" select="$unknown-ram-unknown" />
    <xsl:variable name="unknown-rom-total" select="$unknown-rom-unknown" />
    <xsl:variable name="unknown-total-total" select="$unknown-ram-unknown + $unknown-rom-unknown" />
    <xsl:variable name="total-ram-unknown" select="$unknown-ram-total" />
    <xsl:variable name="total-rom-unknown" select="$unknown-rom-total" />
    <xsl:variable name="total-ram-total" select="$total-ram-variable + $total-ram-function + $total-ram-unknown" />
    <xsl:variable name="total-rom-total" select="$total-rom-variable + $total-rom-function + $total-rom-unknown" />
    <xsl:variable name="total-total-unknown" select="$unknown-total-total" />
    <xsl:variable name="total-total-total" select="$total-total-variable + $total-total-function + $total-total-unknown" />

    <table class="table-1">
      <thead class="table-1">
        <tr>
          <th>
            <xsl:text></xsl:text>
          </th>
          <xsl:for-each select="document('')/*/rd:HeaderList/Sources/Source">
            <th colspan="3">
              <xsl:value-of select="@Name" />
            </th>
          </xsl:for-each>
        </tr>
        <tr>
          <th>
            <xsl:text></xsl:text>
          </th>
          <xsl:for-each select="document('')/*/rd:HeaderList/Sources/Source">
            <xsl:for-each select="document('')/*/rd:HeaderList/Memories/Memory">
              <th>
                <xsl:value-of select="@Name" />
              </th>
            </xsl:for-each>
          </xsl:for-each>
        </tr>
      </thead>
      <tbody class="table-1">
        <tr>
          <td>
            <xsl:text>Variables</xsl:text>
          </td>

          <!-- Generic Library -->
          <td>
            <xsl:value-of select="maksym:HexToDec(@GenericRamVariable)" />
          </td>
          <td>
            <xsl:value-of select="maksym:HexToDec(@GenericRomVariable)" />
          </td>
          <td>
            <xsl:value-of select="$generic-total-variable" />
          </td>

          <!-- System Library -->
          <td>
            <xsl:value-of select="maksym:HexToDec(@SystemRamVariable)"/>
          </td>
          <td>
            <xsl:value-of select="maksym:HexToDec(@SystemRomVariable)" />
          </td>
          <td>
            <xsl:value-of select="$system-total-variable" />
          </td>

          <!-- Application -->
          <td>
            <xsl:value-of select="maksym:HexToDec(@ApplicationRamVariable)"/>
          </td>
          <td>
            <xsl:value-of select="maksym:HexToDec(@ApplicationRomVariable)" />
          </td>
          <td>
            <xsl:value-of select="$application-total-variable" />
          </td>

          <!-- Unknown -->
          <td>
            <xsl:value-of select="$unknown-ram-variable"/>
          </td>
          <td>
            <xsl:value-of select="$unknown-rom-variable"/>
          </td>
          <td>
            <xsl:value-of select="$unknown-total-variable" />
          </td>

          <!-- Total -->
          <td>
            <xsl:value-of select="$total-ram-variable" />
          </td>
          <td>
            <xsl:value-of select="$total-rom-variable" />
          </td>
          <td>
            <xsl:value-of select="$total-total-variable "/>
          </td>
        </tr>
        <tr>
          <td>
            <xsl:text>Functions</xsl:text>
          </td>

          <!-- Generic Library -->
          <td>
            <xsl:value-of select="maksym:HexToDec(@GenericRamFunction)" />
          </td>
          <td>
            <xsl:value-of select="maksym:HexToDec(@GenericRomFunction)" />
          </td>
          <td>
            <xsl:value-of select="$generic-total-function" />
          </td>

          <!-- System Library -->
          <td>
            <xsl:value-of select="maksym:HexToDec(@SystemRamFunction)" />
          </td>
          <td>
            <xsl:value-of select="maksym:HexToDec(@SystemRomFunction)" />
          </td>
          <td>
            <xsl:value-of select="$system-total-function" />
          </td>

          <!-- Application -->
          <td>
            <xsl:value-of select="maksym:HexToDec(@ApplicationRamFunction)" />
          </td>
          <td>
            <xsl:value-of select="maksym:HexToDec(@ApplicationRomFunction)" />
          </td>
          <td>
            <xsl:value-of select="$application-total-function" />
          </td>

          <!-- Unknown -->
          <td>
            <xsl:value-of select="$unknown-ram-function" />
          </td>
          <td>
            <xsl:value-of select="$unknown-rom-function" />
          </td>
          <td>
            <xsl:value-of select="$unknown-total-function" />
          </td>

          <!-- Total -->
          <td>
            <xsl:value-of select="$total-ram-function" />
          </td>
          <td>
            <xsl:value-of select="$total-rom-function" />
          </td>
          <td>
            <xsl:value-of select="$total-total-function" />
          </td>
        </tr>
        <tr>
          <td>
            <xsl:text>Unknown</xsl:text>
          </td>

          <!-- Generic Library -->
          <td>
            <xsl:value-of select="$generic-ram-unknown" />
          </td>
          <td>
            <xsl:value-of select="$generic-rom-unknown" />
          </td>
          <td>
            <xsl:value-of select="$generic-total-unknown" />
          </td>

          <!-- System Library -->
          <td>
            <xsl:value-of select="$system-ram-unknown" />
          </td>
          <td>
            <xsl:value-of select="$system-rom-unknown" />
          </td>
          <td>
            <xsl:value-of select="$system-total-unknown" />
          </td>

          <!-- Application -->
          <td>
            <xsl:value-of select="$application-ram-unknown" />
          </td>
          <td>
            <xsl:value-of select="$application-rom-unknown" />
          </td>
          <td>
            <xsl:value-of select="$application-total-unknown" />
          </td>

          <!-- Unknown -->
          <td>
             <xsl:value-of select="$unknown-ram-unknown" />
          </td>
          <td>
            <xsl:value-of select="$unknown-rom-unknown" />
          </td>
          <td>
            <xsl:value-of select="$unknown-total-unknown" />
          </td>

          <!-- Total -->
          <td>
            <xsl:value-of select="$total-ram-unknown" />
          </td>
          <td>
            <xsl:value-of select="$total-rom-unknown" />
          </td>
          <td>
            <xsl:value-of select="$total-total-unknown" />
          </td>
        </tr>
        <tr>
          <td>
            <xsl:text>Total Size</xsl:text>
          </td>

          <!-- Generic Library -->
          <td>
            <xsl:value-of select="$generic-ram-total" />
          </td>
          <td>
            <xsl:value-of select="$generic-rom-total" />
          </td>
          <td>
            <xsl:value-of select="$generic-total-total" />
          </td>

          <!-- System Library -->
          <td>
            <xsl:value-of select="$system-ram-total" />
          </td>
          <td>
            <xsl:value-of select="$system-rom-total" />
          </td>
          <td>
            <xsl:value-of select="$system-total-total" />
          </td>

          <!-- Application -->
          <td>
            <xsl:value-of select="$application-ram-total" />
          </td>
          <td>
            <xsl:value-of select="$application-rom-total" />
          </td>
          <td>
            <xsl:value-of select="$application-total-total" />
          </td>

          <!-- Unknown -->
          <td>
            <xsl:value-of select="$unknown-ram-total" />
          </td>
          <td>
            <xsl:value-of select="$unknown-rom-total" />
          </td>
          <td>
            <xsl:value-of select="$unknown-total-total" />
          </td>

          <!-- Total -->
          <td>
            <xsl:value-of select="$total-ram-total" />
          </td>
          <td>
            <xsl:value-of select="$total-rom-total" />
          </td>
          <td>
            <xsl:value-of select="$total-total-total" />
          </td>
        </tr>
      </tbody>
    </table>
  </xsl:template>
  
  <xsl:template name="section-list">
    <h2>
      <xsl:attribute name="id">
        <xsl:value-of select="'section-list'"/>
      </xsl:attribute>
      Memory Sections
    </h2>
    <table class="table-2">
      <thead class="table-2">
        <tr>
          <th>
            #
          </th>
          <th>
            Name
          </th>
          <th>
            Start
          </th>
          <th>
            End
          </th>
          <th>
            Size
          </th>
          <th>
            Flag
          </th>
          <th>
            Memory
          </th>
        </tr>
      </thead>
      <tbody class="table-2">
        <xsl:for-each select="//Section">
          <xsl:sort select="@Location" />
          <xsl:variable name="secStart" select="maksym:HexToDec(@Location)" />
          <xsl:variable name="secSize" select="maksym:HexToDec(@Size)" />
          <xsl:variable name="secEnd" select="$secStart + ($secSize - 1)" />
          <tr>
            <xsl:if test="position() mod 2 = 0">
              <xsl:attribute name="bgcolor">#E5E4E2</xsl:attribute>
            </xsl:if>
            <td>
              <a>
                <xsl:attribute name="href">
                  <xsl:value-of select="concat('#', @Location)" />
                </xsl:attribute>
                <xsl:value-of select="position()" />
              </a>
            </td>
            <td>
              <xsl:value-of select="@Name" />
            </td>			
            <td>
              <xsl:value-of select="maksym:DecToHex($secStart)" />
            </td>
            <td>
              <xsl:choose>
                <xsl:when test="$secEnd &lt; 0">
                  <xsl:value-of select="maksym:DecToHex('0')" />
                </xsl:when>
                <xsl:otherwise>
                  <xsl:value-of select="maksym:DecToHex($secEnd)" />
                </xsl:otherwise>
              </xsl:choose>			
            </td>
            <td>
              <xsl:value-of select="$secSize" />
            </td>
            <td>
              <xsl:value-of select="@Flag" />
            </td>
            <td>
              <xsl:choose>
                <xsl:when test="@Flag = 'WRITE'">
                  RAM
                </xsl:when>
                <xsl:when test="@Flag = 'WRITE_ALLOC'">
                  RAM
                </xsl:when>
                <xsl:otherwise>
                  ROM
                </xsl:otherwise>
              </xsl:choose>
            </td>
          </tr>
        </xsl:for-each>
      </tbody>
    </table>
    <table class="legend">
      <tbody class="legend">
        <tr>
          <td>
            WRITE - The section contains data that should be writable during process execution.
          </td>
        </tr>
        <tr>
          <td>
            ALLOC - The section occupies memory during process execution.
          </td>
        </tr>
        <tr>
          <td>
            EXECINSTR - The section contains executable machine instructions.
          </td>
        </tr>
      </tbody>
    </table>    
  </xsl:template>
  
  <xsl:template name="symbol-section-mapping">
    <h2>
      <xsl:attribute name="id">
        <xsl:value-of select="'symbol-section-mapping'"/>
      </xsl:attribute>
      Symbol to Section Mapping
    </h2>
    <xsl:for-each select="//Section">
      <xsl:sort select="@Location" />
      
      <xsl:variable name="secStart" select="maksym:HexToDec(@Location)" />
      <xsl:variable name="secSize" select="maksym:HexToDec(@Size)" />
      <xsl:variable name="secEnd" select="$secStart + ($secSize - 1)" />
      
      <table class="table-3">
        <thead class="table-3">
          <tr>
            <th colspan="5">
              <xsl:attribute name="id">
                <xsl:value-of select="@Location" />
              </xsl:attribute>
              <xsl:value-of select="position()" />
              <xsl:text>: </xsl:text>
			  <xsl:value-of select="@Name" />
			  <xsl:text>, </xsl:text>
              <xsl:choose>
                <xsl:when test="$secEnd &lt; 0">
                  <xsl:value-of select="concat(maksym:DecToHex($secStart), ' - ', maksym:DecToHex('0'))" />
                </xsl:when>
                <xsl:otherwise>
                  <xsl:value-of select="concat(maksym:DecToHex($secStart), ' - ', maksym:DecToHex($secEnd))" />
                </xsl:otherwise>
              </xsl:choose>			  
              <xsl:text>, </xsl:text>
              <xsl:value-of select="$secSize" />
              <xsl:text> Bytes</xsl:text>
            </th>
          </tr>
          <tr>
            <th>
              <xsl:text>Type</xsl:text>
            </th>
            <th>
              <xsl:text>Name</xsl:text>
            </th>
            <th>
              <xsl:text>Start</xsl:text>
            </th>
            <th>
              <xsl:text>End</xsl:text>
            </th>
            <th>
              <xsl:text>Size</xsl:text>
            </th>
          </tr>
        </thead>
        <tbody class="table-3">
          <xsl:value-of select="maksym:SetPosition(1)" />
          <xsl:value-of select="maksym:SetBgColor('#E5E4E2')" />
          <xsl:value-of select="maksym:SetStartAddress(0)" />
          
          <xsl:for-each select="//Function | //Variable">
            <xsl:sort select="@Location" />

            <xsl:variable name="symStart" select="maksym:HexToDec(@Location)" />
            <xsl:variable name="symSize" select="maksym:HexToDec(@Size)" />

            <xsl:if test="$symStart &gt;= $secStart and $symStart &lt;= $secEnd">

              <xsl:variable name="symEnd" select="$symStart + ($symSize - 1)" />
              <xsl:variable name="nextAddr" select="maksym:GetNextAddress()" />
              <xsl:value-of select="maksym:SetNextAddress($symEnd + 1)" />

              <xsl:if test="$nextAddr &lt; $symStart and $nextAddr &gt;= $secStart">
                <xsl:variable name="gapSize" select="$symStart - $nextAddr" />
                <tr>
                  <xsl:attribute name="style">color:red</xsl:attribute>
                  <xsl:choose>
                    <xsl:when test="maksym:GetSize() = 0">
                      <xsl:if test="(maksym:GetPosition() - 1) mod 2 = 0">
                        <xsl:attribute name="bgcolor">
                          <xsl:value-of select="maksym:GetBgColor()" />
                        </xsl:attribute>
                      </xsl:if>
                    </xsl:when>
                    <xsl:otherwise>
                      <xsl:if test="maksym:GetPosition() mod 2 = 0">
                        <xsl:attribute name="bgcolor">
                          <xsl:value-of select="maksym:GetBgColor()" />
                        </xsl:attribute>
                      </xsl:if>
                      <xsl:value-of select="maksym:SetPosition(maksym:GetPosition() + 1)" />
                    </xsl:otherwise>
                  </xsl:choose>
                  <td>
                    <xsl:text>Unknown</xsl:text>
                  </td>
                  <td>
                    <xsl:text>Unknown</xsl:text>
                  </td>
                  <td>
                    <xsl:value-of select="maksym:DecToHex($nextAddr)" />
                  </td>
                  <td>
                    <xsl:value-of select="maksym:DecToHex($nextAddr + ($gapSize - 1))" />
                  </td>
                  <td>
                    <xsl:value-of select="$gapSize" />
                  </td>
                </tr>
              </xsl:if>

              <tr>
                <xsl:if test="maksym:GetPosition() mod 2 = 0">
                  <xsl:attribute name="bgcolor">
                    <xsl:value-of select="maksym:GetBgColor()" />
                  </xsl:attribute>
                </xsl:if>
                <xsl:if test="($symSize = 0) or ($symStart = maksym:GetStartAddress() and position() > 1)">
                  <xsl:attribute name="style">color:red</xsl:attribute>
                </xsl:if>
                <xsl:value-of select="maksym:SetSize($symSize)" />
                <xsl:value-of select="maksym:SetPosition(maksym:GetPosition() + 1)" />
                <td>
                  <xsl:value-of select="name(.)" />
                </td>
                <td>
                  <xsl:value-of select="@Name" />
                </td>
                <td>
                  <xsl:value-of select="maksym:DecToHex($symStart)" />
                </td>
                <td>
                  <xsl:choose>
                    <xsl:when test="$symSize = 0">
                      <xsl:value-of select="'?'" />
                    </xsl:when>
                    <xsl:otherwise>
                      <xsl:value-of select="maksym:DecToHex($symEnd)" />
                    </xsl:otherwise>
                  </xsl:choose>
                </td>
                <td class="symTdSize">
                  <xsl:choose>
                    <xsl:when test="$symSize = 0">
                      <xsl:value-of select="'?'" />
                    </xsl:when>
                    <xsl:otherwise>
                      <xsl:value-of select="$symSize" />
                    </xsl:otherwise>
                  </xsl:choose>
                </td>
              </tr>

              <xsl:value-of select="maksym:SetStartAddress($symStart)" />  
          
            </xsl:if>
          </xsl:for-each>
        </tbody>
      </table>
    </xsl:for-each>  
  </xsl:template>
  
  <xsl:template name="module-list">
    <h2>
      <xsl:attribute name="id">
        <xsl:value-of select="'module-list'"/>
      </xsl:attribute>
      Modules
    </h2>
    <table class="table-2">
      <thead class="table-2">
        <tr>
          <th>
            #
          </th>
          <th>
            Type
          </th>          
          <th>
            File Path
          </th>
        </tr>
      </thead>
      <tbody class="table-2">
        <xsl:for-each select="//CompilationUnit[(@CUSource = 'Generic' or @CUSource = 'System' or @CUSource = 'Application') and (Function or Variable)]">
          <xsl:sort select="concat(@Path, '\', @Name)" />
          <tr>
            <xsl:if test="position() mod 2 = 0">
              <xsl:attribute name="bgcolor">#E5E4E2</xsl:attribute>
            </xsl:if>
            <td>
              <a>
                <xsl:attribute name="href">
                  <xsl:value-of select="concat('#', @Path, '\', @Name)"/>
                </xsl:attribute>
                <xsl:value-of select="position()" />
              </a>
            </td>
            <td>
              <xsl:value-of select="@CUSource" />
            </td>
            <td>
              <xsl:attribute name="style">text-align:left</xsl:attribute> 
              <xsl:value-of select="concat(@Path, '\', @Name)" />
            </td>
          </tr>
        </xsl:for-each>
      </tbody>
    </table>
  </xsl:template>
  
  <xsl:template name="symbol-module-mapping">
    <h2>
      <xsl:attribute name="id">
        <xsl:value-of select="'symbol-module-mapping'"/>
      </xsl:attribute>
      Symbol to Module Mapping
    </h2>
    <xsl:for-each select="//CompilationUnit[(@CUSource = 'Generic' or @CUSource = 'System' or @CUSource = 'Application') and (Function or Variable)]">
      <xsl:sort select="concat(@Path, '\', @Name)" />
      <table class="table-4">
        <tbody class="table-4">
          <tr>
            <td colspan="2">
              <xsl:apply-templates select="Size">
                <xsl:with-param name="id" select="position()" />
                <xsl:with-param name="cu-source" select="@CUSource" />
                <xsl:with-param name="path" select="concat(@Path, '\', @Name)" />
              </xsl:apply-templates>
            </td>
          </tr>
          <tr>
            <td>
              <xsl:call-template name="variable-list" />
            </td>
            <td>
              <xsl:call-template name="function-list" />
            </td>
          </tr>
        </tbody>
      </table>
    </xsl:for-each>      
  </xsl:template>

  <xsl:template match="//CompilationUnit/Size">
    <xsl:param name="id" />
    <xsl:param name="cu-source" />
    <xsl:param name="path" />

    <xsl:variable name="total-ram-variable" select="maksym:HexToDec(@GenericRamVariable) + maksym:HexToDec(@SystemRamVariable) + maksym:HexToDec(@ApplicationRamVariable)" />
    <xsl:variable name="total-rom-variable" select="maksym:HexToDec(@GenericRomVariable) + maksym:HexToDec(@SystemRomVariable) + maksym:HexToDec(@ApplicationRomVariable)" />
    <xsl:variable name="total-total-variable" select="$total-ram-variable + $total-rom-variable" />

    <xsl:variable name="total-ram-function" select="maksym:HexToDec(@GenericRamFunction) + maksym:HexToDec(@SystemRamFunction) + maksym:HexToDec(@ApplicationRamFunction)" />
    <xsl:variable name="total-rom-function" select="maksym:HexToDec(@GenericRomFunction) + maksym:HexToDec(@SystemRomFunction) + maksym:HexToDec(@ApplicationRomFunction)" />
    <xsl:variable name="total-total-function" select="$total-ram-function + $total-rom-function" />

    <xsl:variable name="total-ram-total" select="$total-ram-variable + $total-ram-function" />
    <xsl:variable name="total-rom-total" select="$total-rom-variable + $total-rom-function" />
    <xsl:variable name="total-total-total" select="$total-total-variable + $total-total-function" />
    <table class="table-5">
      <thead class="table-5">
        <tr>
          <th colspan="4">
            <xsl:attribute name="id">
              <xsl:value-of select="$path"/>
            </xsl:attribute>
            <xsl:value-of select="concat($id, ': ', $cu-source, ', ', $path)" />
          </th>
        </tr>
        <tr>
          <th>
            <xsl:text></xsl:text>
          </th>
          <xsl:for-each select="document('')/*/rd:HeaderList/Sources/Source">
            <xsl:if test="@Name = 'Total'">
              <xsl:for-each select="document('')/*/rd:HeaderList/Memories/Memory">
                <th>
                  <xsl:value-of select="@Name" />
                </th>
              </xsl:for-each>
            </xsl:if>
          </xsl:for-each>
        </tr>
      </thead>
      <tbody class="table-5">
        <tr>
          <td>
            <xsl:text>Variables</xsl:text>
          </td>

          <!-- Total -->
          <td>
            <xsl:value-of select="$total-ram-variable" />
          </td>
          <td>
            <xsl:value-of select="$total-rom-variable" />
          </td>
          <td>
            <xsl:value-of select="$total-total-variable "/>
          </td>
        </tr>
        <tr>
          <td>
            <xsl:text>Functions</xsl:text>
          </td>

          <!-- Total -->
          <td>
            <xsl:value-of select="$total-ram-function" />
          </td>
          <td>
            <xsl:value-of select="$total-rom-function" />
          </td>
          <td>
            <xsl:value-of select="$total-total-function" />
          </td>
        </tr>
        <tr>
          <td>
            <xsl:text>Total Size</xsl:text>
          </td>

          <!-- Total -->
          <td>
            <xsl:value-of select="$total-ram-total" />
          </td>
          <td>
            <xsl:value-of select="$total-rom-total" />
          </td>
          <td>
            <xsl:value-of select="$total-total-total" />
          </td>
        </tr>
      </tbody>
    </table>
  </xsl:template>

  <xsl:template name="variable-list">
   <xsl:param name="path" />
    
   <xsl:variable name="variable-size" select="maksym:HexToDec(Size/@GenericRamVariable) + maksym:HexToDec(Size/@GenericRomVariable)
                  + maksym:HexToDec(Size/@SystemRamVariable) + maksym:HexToDec(Size/@SystemRomVariable)
                  + maksym:HexToDec(Size/@ApplicationRamVariable) + maksym:HexToDec(Size/@ApplicationRomVariable)"/>
    <xsl:if test="$variable-size != -1">
      <table class="table-6">
        <thead class="table-6">
          <tr>
            <th colspan="4">
              <xsl:text>Variables</xsl:text>
            </th>
          </tr>
          <tr>
            <th>
              Name
            </th>
            <th>
              Location
            </th>
            <th>
              Size
            </th>
            <th>
              Memory
            </th>
          </tr>
        </thead>
        <tbody class="table-6">
          <xsl:for-each select="Variable">
            <!-- <xsl:sort select="@Name"/> -->
            <xsl:sort select="maksym:HexToDec(@Location)" data-type="number" order="ascending" />
            <xsl:call-template name="variable-row"/>
          </xsl:for-each>
        </tbody>
      </table>
    </xsl:if>
  </xsl:template>

  <xsl:template name="function-list">
    <xsl:variable name="FunctionSize" select="maksym:HexToDec(Size/@GenericRamFunction) + maksym:HexToDec(Size/@GenericRomFunction)
                  + maksym:HexToDec(Size/@SystemRamFunction) + maksym:HexToDec(Size/@SystemRomFunction)
                  + maksym:HexToDec(Size/@ApplicationRamFunction) + maksym:HexToDec(Size/@ApplicationRomFunction)"/>
    <xsl:if test="$FunctionSize != -1">
      <table class="table-6">
        <thead class="table-6">
          <tr>
            <th colspan="4">
              <xsl:text>Functions</xsl:text>
            </th>
          </tr>
          <tr>
            <th>
              Name
            </th>
            <th>
              Location
            </th>
            <th>
              Size
            </th>
            <th>
              Memory
            </th>
          </tr>
        </thead>
        <tbody class="table-6">
          <xsl:value-of select="maksym:SetNextAddress(0)" />
          <xsl:for-each select="Function">
            <!-- <xsl:sort select="@Name"/> -->
            <xsl:sort select="maksym:HexToDec(@Location)" data-type="number" order="ascending" />
            <xsl:call-template name="function-row"/>
            <xsl:value-of select="maksym:SetNextAddress(maksym:HexToDec(@Location))" />
          </xsl:for-each>
        </tbody>
      </table>
    </xsl:if>
  </xsl:template>

  <xsl:template name="variable-row">
    <tr>
      <xsl:if test="position() mod 2 = 0">
        <xsl:attribute name="bgcolor">#E5E4E2</xsl:attribute>
      </xsl:if>
      <td>
        <xsl:value-of select="@Name"/>
      </td>
      <td>
        <xsl:value-of select="@Location"/>
      </td>
      <td>
        <xsl:value-of select="maksym:HexToDec(@Size)"/>
      </td>
      <td>
        <xsl:value-of select="@Memory"/>
      </td>
    </tr>    
  </xsl:template>

  <xsl:template name="function-row">
    <tr>
      <xsl:if test="position() mod 2 = 0">
        <xsl:attribute name="bgcolor">#E5E4E2</xsl:attribute>
      </xsl:if>
      <xsl:if test="maksym:HexToDec(@Location) = maksym:GetNextAddress()">
        <xsl:attribute name="style">color:red</xsl:attribute>
      </xsl:if>
      <td>
        <xsl:value-of select="@Name"/>
      </td>
      <td>
        <xsl:value-of select="@Location"/>
      </td>
      <td>
        <xsl:value-of select="maksym:HexToDec(@Size)"/>
      </td>
      <td>
        <xsl:value-of select="@Memory"/>
      </td>
    </tr>
  </xsl:template>
  
</xsl:stylesheet>
