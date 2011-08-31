<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
	<html>
		<head>
			<title>SORE log</title>
			<style type="text/css" title="currentStyle" media="screen">
				@import "style.css";
			</style>
			<script src="dynamic_log.js" type="text/javascript" language="Javascript"></script>
		</head>
		<body onload="DynamicLog();">
		<h2>SORE log</h2>
		<div id="log">
		<xsl:for-each select="log/function">
			<div class="container">
			<xsl:if test="name = ''">
				<h3>(No function)</h3>
			</xsl:if>
			<xsl:if test="name != ''">
				<h3><xsl:value-of select="name"/></h3>
			</xsl:if>
			<div class="messages">
			<xsl:for-each select="message">
				<xsl:if test="level = 1">
					<div class = "critical">
						<small><b>Time: </b><xsl:value-of select="time"/></small>
						<small><b>Severity: </b><xsl:value-of select="levelstr"/></small>
						<xsl:if test="line != 0">
							<small><b>Line: </b><xsl:value-of select="line"/></small>
						</xsl:if>
						<xsl:if test="file != '(null)'">
							<small><b>File: </b><xsl:value-of select="file"/></small>
						</xsl:if>
						<div class="message"><xsl:call-template name="CopyWithLineBreaks">
	<xsl:with-param name="string" select="data"/>
</xsl:call-template></div>
					</div>
				</xsl:if> 
				<xsl:if test="level = 2">
					<div class = "error">
						<small><b>Time: </b><xsl:value-of select="time"/></small>
						<small><b>Severity: </b><xsl:value-of select="levelstr"/></small>
						<xsl:if test="line != 0">
							<small><b>Line: </b><xsl:value-of select="line"/></small>
						</xsl:if>
						<xsl:if test="file != '(null)'">
							<small><b>File: </b><xsl:value-of select="file"/></small>
						</xsl:if>
						<div class="message"><xsl:call-template name="CopyWithLineBreaks">
	<xsl:with-param name="string" select="data"/>
</xsl:call-template></div>
					</div>
				</xsl:if> 
				<xsl:if test="level = 4">
					<div class = "warning">
						<small><b>Time: </b><xsl:value-of select="time"/></small>
						<small><b>Severity: </b><xsl:value-of select="levelstr"/></small>
						<xsl:if test="line != 0">
							<small><b>Line: </b><xsl:value-of select="line"/></small>
						</xsl:if>
						<xsl:if test="file != '(null)'">
							<small><b>File: </b><xsl:value-of select="file"/></small>
						</xsl:if>
						<div class="message"><xsl:call-template name="CopyWithLineBreaks">
	<xsl:with-param name="string" select="data"/>
</xsl:call-template></div>
					</div>
				</xsl:if> 
				<xsl:if test="level = 8">
					<div class = "info">
						<small><b>Time: </b><xsl:value-of select="time"/></small>
						<small><b>Severity: </b><xsl:value-of select="levelstr"/></small>
						<xsl:if test="line != 0">
							<small><b>Line: </b><xsl:value-of select="line"/></small>
						</xsl:if>
						<xsl:if test="file != '(null)'">
							<small><b>File: </b><xsl:value-of select="file"/></small>
						</xsl:if>
						<div class="message">
<xsl:call-template name="CopyWithLineBreaks">
	<xsl:with-param name="string" select="data"/>
</xsl:call-template></div>
					</div>
				</xsl:if>
				<xsl:if test="level = 16">
					<div class = "debug1">
						<small><b>Time: </b><xsl:value-of select="time"/></small>
						<small><b>Severity: </b><xsl:value-of select="levelstr"/></small>
						<xsl:if test="line != 0">
							<small><b>Line: </b><xsl:value-of select="line"/></small>
						</xsl:if>
						<xsl:if test="file != '(null)'">
							<small><b>File: </b><xsl:value-of select="file"/></small>
						</xsl:if>
						<div class="message"><xsl:call-template name="CopyWithLineBreaks">
	<xsl:with-param name="string" select="data"/>
</xsl:call-template></div>
					</div>
				</xsl:if> 
				<xsl:if test="level = 32">
					<div class = "debug2">
						<small><b>Time: </b><xsl:value-of select="time"/></small>
						<small><b>Severity: </b><xsl:value-of select="levelstr"/></small>
						<xsl:if test="line != 0">
							<small><b>Line: </b><xsl:value-of select="line"/></small>
						</xsl:if>
						<xsl:if test="file != '(null)'">
							<small><b>File: </b><xsl:value-of select="file"/></small>
						</xsl:if>
						<div class="message"><xsl:call-template name="CopyWithLineBreaks">
	<xsl:with-param name="string" select="data"/>
</xsl:call-template></div>
					</div>
				</xsl:if>
			</xsl:for-each>
			</div>
			</div>
		</xsl:for-each>
		</div>
		</body>
	</html>
</xsl:template>

<xsl:template name="lf2br">
	<!-- import $StringToTransform -->
	<xsl:param name="StringToTransform"/>
	<xsl:choose>
		<!-- string contains linefeed -->
		<xsl:when test="contains($StringToTransform,'&#xA;')">
			<!-- output substring that comes before the first linefeed -->
			<!-- note: use of substring-before() function means        -->
			<!-- $StringToTransform will be treated as a string,       -->
			<!-- even if it is a node-set or result tree fragment.     -->
			<!-- So hopefully $StringToTransform is really a string!   -->
			<xsl:value-of select="substring-before($StringToTransform,'&#xA;')"/>
			<!-- by putting a 'br' element in the result tree instead  -->
			<!-- of the linefeed character, a <br> will be output at   -->
			<!-- that point in the HTML                                -->
			<br/>
			<!-- repeat for the remainder of the original string -->
			<xsl:call-template name="lf2br">
				<xsl:with-param name="StringToTransform">
					<xsl:value-of select="substring-after($StringToTransform,'&#xA;')"/>
				</xsl:with-param>
			</xsl:call-template>
		</xsl:when>
		<!-- string does not contain newline, so just output it -->
		<xsl:otherwise>
			<xsl:value-of select="$StringToTransform"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template name="CopyWithLineBreaks">
	<xsl:param name="string"/>
	<xsl:variable name="Result">
		<xsl:call-template name="lf2br">
			<xsl:with-param name="StringToTransform" select="$string"/>
		</xsl:call-template>
	</xsl:variable>
	<xsl:copy-of select="$Result"/>
</xsl:template>

</xsl:stylesheet>