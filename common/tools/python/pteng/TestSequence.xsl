<?xml version="1.0"?>

<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="html" indent="yes"/>

    <xsl:template match="/">
        <xsl:apply-templates select="test_sequences"/>
    </xsl:template>
    
    <xsl:template match="projectname">
        <xsl:value-of select="@project"/>
    </xsl:template>
    
    <xsl:template match="test_timestamp">
        <xsl:apply-templates select="text()"/>
    </xsl:template>
    
    <xsl:template match="test_sequences">
        <html>
        <head>
        <style type="text/css">
            <xsl:comment>
                .literal    { font-family: Courier, monospace; }
                .failed     {background-color: #d14a32;}
                .error      {background-color: #af3c27;}
                .disabled   {background-color: #bca41b;}
                .passed     {background-color: #5aab9b;}
                .global     {font-size: 50 px; background-color: lightgrey;}
                .TestSequence {font-size: 40 px; background-color: lightblue;}
                .TestCase   {font-size: 35 px; background-color: lightgreen;}
                .ws {white-space: pre-wrap;}
                .testcasenumber {color: lightyellow; font-size: 30px;}
                .TestSequenceName {background-color: lightblue; color: yellow; font-size: 28px;}
            </xsl:comment>
        </style>
        </head>
            <center><h2><xsl:apply-templates select="projectname"/></h2></center>
        <body>
            <center>
            <table border="1" class="global">
            <tr>
                <td>Executed:<xsl:apply-templates select="test_timestamp"/></td>  
                <xsl:apply-templates select="test_sequence_result"/>
            </tr>
            <tr>
               <td>CodeCoverage of Sequences:</td>
               <td>
                   <xsl:value-of select="projectname/@sequences_covr" disable-output-escaping="yes"/>
               </td>
            </tr>
            <tr>
               <td colspan="2">
                    <xsl:value-of select="projectname/@connection" disable-output-escaping="yes"/>
               </td>
            </tr>
            <tr>
                <td colspan="2">
                    <center><xsl:apply-templates select="test_sequence"/></center>
                </td>
            </tr>
            </table>
            </center>
        </body>
    </html>
    </xsl:template>

    <xsl:template match="test_sequence">
    <table border="1" width="100%" cellspacing="1" cellpadding="1" class="TestSequence">
        <tr>
            <td>TestSequence:</td>
            <td class="TestSequenceName"><xsl:value-of select="@identifier"/></td>  
        </tr>
        <tr>
            <td>FunctionCoverage:</td>
            <td class="SequenceCodeCoverage"><xsl:value-of select="function_covr/text()"/></td>  
        </tr>
        <tr>
            <td>Unit:</td>
            <td><xsl:value-of select="unitname/@unit"/></td>  
        </tr>
        <tr>
            <td>Description:</td>
            <td><xsl:apply-templates select="description"/></td>  
        </tr>
        <tr>
            <td>Function:</td>
            <td><xsl:value-of select="function_sig/text()"/></td>
        </tr>
        <tr>
            <td colspan="2"><xsl:apply-templates select="test_cases"/></td>        
        </tr>
    </table>
    </xsl:template>

    <xsl:template match="function">
        <xsl:value-of select="function_sig/text()"/>
    </xsl:template>

    <xsl:template match="test_cases">
        <xsl:apply-templates select="test_case"/>
    </xsl:template>

    <xsl:template match="test_case">
        <table border="1" width="100%" cellspacing="1" cellpadding="1" class="TestCase">
        <tr>
            <td>Testcase:</td>
            <td align="center" class="testcasenumber" colspan="2"><xsl:value-of select="@identifier"/></td>
        </tr>
        <tr>
            <td>StartTime:</td>
            <td colspan="2"><xsl:value-of select="start_time/text()"/></td>
        </tr>
        <tr>
            <td>TimeDelta:</td>
            <td colspan="2"><xsl:value-of select="delta_time/text()"/></td>
        </tr>
        <tr>
            <td>Validation:</td>
            <xsl:apply-templates select="test_case_result"/>
        </tr>
        <tr>
            <td>Info:</td>
            <td colspan="2"><xsl:value-of select="add_info/text()"/></td>
        </tr>
        
        <tr>
            <td>Comment:</td>
            <td colspan="2"><xsl:apply-templates select="description"/></td>
        </tr>
        
        
        <xsl:choose>
        <xsl:when test="test_parameter|test_return|test_variable">
        <tr>
            <td></td>
            <td><b>in</b></td>
            <td><table border="1" width="100%" cellspacing="2" cellpadding="0"><tr><td width="35%" align="center"><b>outcome</b></td><td width="35%" align="center"><b>expectation</b></td><td width="30%" align="center"><b>evaluation</b></td></tr></table></td>
        </tr>
        </xsl:when>
        </xsl:choose>
        
        <xsl:choose>
        <xsl:when test="test_parameter">
        <tr>
        <td><b>TestParameters</b></td>
        <td colspan="2"><b></b></td>
        </tr> 
        </xsl:when>
        </xsl:choose>
        <xsl:apply-templates select="test_parameter"/>
        
        <xsl:choose>
        <xsl:when test="test_return">
        <tr>
        <td><b>ReturnValues</b></td>
        <td colspan="2"><b></b></td>
        </tr> 
        </xsl:when>
        </xsl:choose>
        <xsl:apply-templates select="test_return"/>
        
        
        
        
        <xsl:choose>
        <xsl:when test="test_variable">
        <tr>
        <td><b>TestVariables</b></td>
        <td colspan="2"><b></b></td>
        </tr> 
        </xsl:when>
        </xsl:choose>
        <xsl:apply-templates select="test_variable"/>
        


        </table>
    </xsl:template>

    <xsl:template match="description">
        <xsl:value-of select="text()"/>
    </xsl:template>

    <xsl:template match="test_parameter">
    <tr> 
    <td><xsl:value-of select="@identifier"/></td>
    <td>
    <table border="1" width="100%" cellspacing="0" cellpadding="0" class="TestCase">
    <tr>
    <td><xsl:value-of select="in/text()"/></td>
    </tr>
    </table>
    </td>
    <td></td>
    </tr>
    </xsl:template>

    <xsl:template match="test_return">
    <tr>
    <td><xsl:value-of select="@identifier"/></td>
    <td></td>
    <td><xsl:apply-templates select="out"/></td>
    </tr>
    </xsl:template>

    <xsl:template match="test_variable">
    <tr> 
    <td><xsl:value-of select="@identifier"/></td>
    <td>
    <table border="1" width="100%" cellspacing="0" cellpadding="0" class="TestCase">
    <tr>
    <td><xsl:value-of select="in/text()"/></td>
    </tr>
    </table>
    </td>
    <td><xsl:apply-templates select="out"/></td>
    </tr>
    <xsl:if test="add_info != ''">
        <tr>
        <td colspan="3"><pre><xsl:value-of select="add_info/text()"/></pre></td>
        </tr>
    </xsl:if>
    </xsl:template>

    <xsl:template match="out">
    <table border="1" width="100%" cellspacing="2" cellpadding="0" class="TestCase">
    <tr>
    <td width="35%"><xsl:value-of select="outcome/text()"/></td>
    <td width="35%">
    <xsl:choose>
    <xsl:when test="equal_to">
    <xsl:text> &#61;&#61; </xsl:text><xsl:value-of select="equal_to/text()"/>
    </xsl:when>
    <xsl:when test="greater_than">
    <xsl:text> &#62; </xsl:text><xsl:value-of select="greater_than/text()"/>
    </xsl:when>
    <xsl:when test="less_than">
    <xsl:text> &#60; </xsl:text><xsl:value-of select="less_than/text()"/>
    </xsl:when>
    <xsl:when test="less_than_or_equal_to">
    <xsl:text> &#60;= </xsl:text><xsl:value-of select="less_than/text()"/>
    </xsl:when>
    <xsl:when test="greater_than_or_equal_to">
    <xsl:text> &#62;= </xsl:text><xsl:value-of select="less_than/text()"/>
    </xsl:when>
    <xsl:when test="equal_to_range">
    <xsl:text> =.. </xsl:text><xsl:value-of select="less_than/text()"/>
    </xsl:when>
    <xsl:when test="not_equal_to_range">
    <xsl:text> !=.. </xsl:text><xsl:value-of select="less_than/text()"/>
    </xsl:when>
    </xsl:choose>
    </td>
    <xsl:apply-templates select="validation"/>
    </tr>
    </table>
    </xsl:template>
    

    <xsl:template match="validation | test_case_result | test_sequence_result">
    
    <xsl:if test="text() = 'Exception'">
    <td class="failed"><center><xsl:value-of select="text()"/></center></td>
    </xsl:if>
    <xsl:if test="text() = 'Passed'">
    <td class="passed"><center><xsl:value-of select="text()"/></center></td>
    </xsl:if>
    <xsl:if test="text() = 'Failed'">
    <td class="failed"><center><xsl:value-of select="text()"/></center></td>
    </xsl:if>
    </xsl:template>

</xsl:stylesheet>
