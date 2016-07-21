<?xml version="1.0"?>

<xsl:stylesheet 
		xmlns="http://schemas.microsoft.com/wix/2003/01/wi"
		xmlns:wix="http://schemas.microsoft.com/wix/2003/01/wi"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
		version="1.0">
	<xsl:param name="IdPrefix" select="''" />
	<xsl:output method="xml" version="1.0" indent="yes" />
	<xsl:include href="string_util.xsl" />
	<xsl:template match="/">
		<Include>
			<xsl:for-each select="wix:Wix/wix:Fragment/wix:DirectoryRef/wix:Directory/wix:Component/wix:File">
				<File>
					<xsl:attribute name="Id">
						<xsl:if test="$IdPrefix != ''">
							<xsl:value-of select="$IdPrefix" />
						</xsl:if>
						<xsl:call-template name="replaceCharsInString">
							<xsl:with-param name="stringIn">
								<xsl:choose>
									<xsl:when test="@LongName">
										<xsl:value-of select="@LongName" />
									</xsl:when>
									<xsl:otherwise>
										<xsl:value-of select="@Name" />
									</xsl:otherwise>
								</xsl:choose>
							</xsl:with-param>
							<xsl:with-param name="charsIn" select="'-'"/>
							<xsl:with-param name="charsOut" select="'_'"/>
						</xsl:call-template>
					</xsl:attribute>
					<xsl:attribute name="Name">
						<xsl:value-of select="@Name" />
					</xsl:attribute>
					<xsl:if test="@LongName">
						<xsl:attribute name="LongName">
							<xsl:value-of select="@LongName" />
						</xsl:attribute>
					</xsl:if>
					<xsl:attribute name="src">
						<xsl:value-of select="@src" />
					</xsl:attribute>
				</File>
			</xsl:for-each>
		</Include>
	</xsl:template>
</xsl:stylesheet>
