<?xml version="1.0"?>

<xsl:stylesheet 
		xmlns="http://schemas.microsoft.com/wix/2003/01/wi"
		xmlns:wix="http://schemas.microsoft.com/wix/2003/01/wi"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
		version="1.0">
	<xsl:param name="IdPrefix" select="''" />
	<xsl:output method="xml" version="1.0" omit-xml-declaration="yes" indent="yes" />
	<xsl:include href="string_util.xsl" />
	<xsl:template match="/">
		<xsl:apply-templates />
	</xsl:template>
	<xsl:template match="/wix:Wix/wix:Fragment">
		<Include>
			<xsl:apply-templates select="wix:Registry" />
		</Include>
	</xsl:template>
	<xsl:template match="wix:Registry">
		<Registry>
			<xsl:if test="not(@Id)">
				<xsl:attribute name="Id">
					<xsl:value-of select="$IdPrefix" />
					<xsl:value-of select="generate-id()" />
				</xsl:attribute>
			</xsl:if>
			<xsl:if test="@Root and @Key and not(@Type) and not(@Value)">
				<!-- retrieve type/value pair from unnamed child -->
				<xsl:for-each select="wix:Registry[not(@Name) or @Name=''][1]">
					<xsl:attribute name="Type"><xsl:value-of select="@Type" /></xsl:attribute>
					<xsl:attribute name="Value">
						<xsl:call-template name="replaceCharsInString">
							<xsl:with-param name="stringIn">
								<xsl:value-of select="@Value" />
							</xsl:with-param>
							<xsl:with-param name="charsIn" select="'\\'"/>
							<xsl:with-param name="charsOut" select="'\'"/>
						</xsl:call-template>
					</xsl:attribute>
				</xsl:for-each>
			</xsl:if>
			<xsl:for-each select="@*">
				<xsl:if test="string(.)!=''">
					<xsl:attribute name="{name()}">
						<xsl:choose>
							<xsl:when test="name() = 'Value'">
								<xsl:call-template name="replaceCharsInString">
									<xsl:with-param name="stringIn">
										<xsl:value-of select="." />
									</xsl:with-param>
									<xsl:with-param name="charsIn" select="'\\'"/>
									<xsl:with-param name="charsOut" select="'\'"/>
								</xsl:call-template>
							</xsl:when>
							<xsl:otherwise>
								<xsl:value-of select="." />
							</xsl:otherwise>
						</xsl:choose>
					</xsl:attribute>
				</xsl:if>
			</xsl:for-each>
			<xsl:apply-templates select="wix:Registry[(@Root!='' and @Key!='') or @Name!='']" />
		</Registry>
	</xsl:template>
</xsl:stylesheet>
