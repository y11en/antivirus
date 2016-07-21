<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
	<xsl:template name="replaceCharsInString">
		<xsl:param name="stringIn"/>
		<xsl:param name="charsIn"/>
		<xsl:param name="charsOut"/>
		<xsl:choose>
			<xsl:when test="contains($stringIn,$charsIn)">
				<xsl:value-of select="concat(substring-before($stringIn,$charsIn),$charsOut)"/>
				<xsl:call-template name="replaceCharsInString">
					<xsl:with-param name="stringIn" select="substring-after($stringIn,$charsIn)"/>
					<xsl:with-param name="charsIn" select="$charsIn"/>
					<xsl:with-param name="charsOut" select="$charsOut"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$stringIn"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
</xsl:stylesheet>
