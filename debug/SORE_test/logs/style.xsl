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
						<div class="message"><xsl:value-of select="data"/></div>
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
						<div class="message"><xsl:value-of select="data"/></div>
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
						<div class="message"><xsl:value-of select="data"/></div>
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
						<div class="message"><xsl:value-of select="data"/></div>
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
						<div class="message"><xsl:value-of select="data"/></div>
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
						<div class="message"><xsl:value-of select="data"/></div>
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

</xsl:stylesheet>