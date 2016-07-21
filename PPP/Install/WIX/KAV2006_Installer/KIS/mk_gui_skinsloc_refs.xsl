<?xml version="1.0"?>

<xsl:stylesheet 
		xmlns="http://schemas.microsoft.com/wix/2003/01/wi"
		xmlns:wix="http://schemas.microsoft.com/wix/2003/01/wi"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
		version="1.0">
	<xsl:output method="xml" version="1.0" indent="yes" />
	<xsl:template match="/">
		<xsl:apply-templates />
	</xsl:template>
	<xsl:template match="/wix:Wix/wix:Fragment/wix:DirectoryRef/wix:Directory">
		<Include>
			<xsl:apply-templates select="wix:Component|wix:Directory" mode="internal" />
		</Include>
	</xsl:template>
	<xsl:template match="wix:Directory" mode="internal">
		<xsl:apply-templates select="wix:Component|wix:Directory" mode="internal" />
	</xsl:template>
	<xsl:template match="wix:Component" mode="internal">
		<ComponentRef>
			<xsl:choose>
				<xsl:when test="../@LongName">
					<xsl:attribute name="Id">SkinLoc_Component_<xsl:value-of select="../@LongName" /></xsl:attribute>
				</xsl:when>
				<xsl:otherwise>
					<xsl:attribute name="Id">SkinLoc_Component_<xsl:value-of select="../@Name" /></xsl:attribute>
				</xsl:otherwise>
			</xsl:choose>
		</ComponentRef>
	</xsl:template>
</xsl:stylesheet>
