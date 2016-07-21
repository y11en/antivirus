<?xml version="1.0"?>

<xsl:stylesheet 
		xmlns="http://schemas.microsoft.com/wix/2003/01/wi"
		xmlns:wix="http://schemas.microsoft.com/wix/2003/01/wi"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
		version="1.0">
	<xsl:output method="xml" version="1.0" indent="yes" />
	<xsl:include href="string_util.xsl" />

	<xsl:template match="/">
		<xsl:apply-templates />
	</xsl:template>
	
	<xsl:template match="/wix:Wix/wix:Fragment/wix:DirectoryRef/wix:Directory">
		<Include>
			<xsl:apply-templates select="wix:Component|wix:Directory" mode="internal" />
		</Include>
	</xsl:template>

	<xsl:template match="wix:Directory" mode="internal">
		<Directory>
			<xsl:for-each select="@*">
				<xsl:if test="string(.)!=''">
					<xsl:choose>
						<xsl:when test="name() = 'Id'">
							<xsl:choose>
								<xsl:when test="../@LongName">
									<xsl:attribute name="Id">SkinLoc<xsl:value-of select="../@LongName" />Dir</xsl:attribute>
								</xsl:when>
								<xsl:otherwise>
									<xsl:attribute name="Id">SkinLoc<xsl:value-of select="../@Name" />Dir</xsl:attribute>
								</xsl:otherwise>
							</xsl:choose>
						</xsl:when>
						<xsl:otherwise>
							<xsl:attribute name="{name()}"><xsl:value-of select="." /></xsl:attribute>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:if>
			</xsl:for-each>
			<xsl:apply-templates select="wix:Component|wix:Directory" mode="internal" />
		</Directory>
	</xsl:template>

	<xsl:template match="wix:Component" mode="internal">
		<Component>
			<xsl:for-each select="@*">
				<xsl:if test="string(.)!=''">
					<xsl:choose>
						<xsl:when test="name() = 'Id'">
							<xsl:choose>
								<xsl:when test="../../@LongName">
									<xsl:attribute name="Id">SkinLoc_Component_<xsl:value-of select="../../@LongName" /></xsl:attribute>
									<xsl:attribute name="Guid">$(var.SkinLoc_Component_<xsl:value-of select="../../@LongName" />_Guid)</xsl:attribute>
								</xsl:when>
								<xsl:otherwise>
									<xsl:attribute name="Id">SkinLoc_Component_<xsl:value-of select="../../@Name" /></xsl:attribute>
									<xsl:attribute name="Guid">$(var.SkinLoc_Component_<xsl:value-of select="../../@Name" />_Guid)</xsl:attribute>
								</xsl:otherwise>
							</xsl:choose>
						</xsl:when>
						<xsl:otherwise>
							<xsl:attribute name="{name()}"><xsl:value-of select="." /></xsl:attribute>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:if>
			</xsl:for-each>
			<xsl:apply-templates select="wix:File" mode="internal" />
			<RemoveFile>
				<xsl:choose>
					<xsl:when test="../@LongName">
						<xsl:attribute name="Id">SkinLoc_Component_<xsl:value-of select="../@LongName" />_RemoveFiles</xsl:attribute>
					</xsl:when>
					<xsl:otherwise>
						<xsl:attribute name="Id">SkinLoc_Component_<xsl:value-of select="../@Name" />_RemoveFiles</xsl:attribute>
					</xsl:otherwise>
				</xsl:choose>
				<xsl:attribute name="Name">*.*</xsl:attribute>
				<xsl:attribute name="On">uninstall</xsl:attribute>
			</RemoveFile>
		</Component>
	</xsl:template>

	<xsl:template match="wix:File" mode="internal">
		<xsl:if test="not(contains(@Name, '.scc'))">
			<File>
				<xsl:attribute name="Id">
					<xsl:value-of select="../../@Name" />
					<xsl:value-of select="'Loc_'" />
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
		</xsl:if>
	</xsl:template>

</xsl:stylesheet>
