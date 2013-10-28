/****************************************************************************
**
** Copyright (C) 2013, HicknHack Software
** All rights reserved.
** Contact: http://www.hicknhack-software.com/contact
**
** This file is part of the QSqlMigrator
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL3 included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/
#include "PostgresqlMigrator/Helper/PostgresqlColumnService.h"

#include "PostgresqlMigrator/Helper/PostgresqlTypeMapperService.h"

#include <QStringList>

#include <QDebug>

using namespace Structure;

namespace Helper {

PostgresqlColumnService::PostgresqlColumnService()
{
}

QString PostgresqlColumnService::generateColumnDefinitionSql(const Column &column) const
{
    QStringList sqlColumnOptions;
    if (column.isPrimary()) {
        sqlColumnOptions << "PRIMARY KEY";
    }
    QString sqlTypeString;
    bool serial = false;
    if (column.isAutoIncremented()) {
        // PostgreSQL has no auto increment, instead there are serials (as types)
        // QVariant has no Short
        if (column.sqlType().type() == QVariant::Int)
            sqlTypeString = "SERIAL";
        else if (column.sqlType().type() == QVariant::LongLong)
            sqlTypeString = "BIGSERIAL";
        else {
            qWarning() << "column" << column.name() << "has auto increment specified but is not of integer type";
            sqlTypeString = "SERIAL";
        }
        serial = true;
    } else {
        if (column.hasSqlTypeString())
            sqlTypeString = column.sqlTypeString();
        else {
            PostgresqlTypeMapperService typeMapperService;
            sqlTypeString = typeMapperService.map(column.sqlType());
        }
    }
    // primary keys are implicit NOT NULL, serials are already NOT NULL
    if (!column.isPrimary() && !serial && !column.isNullable()) {
        sqlColumnOptions << "NOT NULL";
    }
    if (column.isUnique()) {
        sqlColumnOptions << "UNIQUE";
    }
    if (column.hasDefaultValue()) {
        sqlColumnOptions << QString("DEFAULT (%1)").arg(column.defaultValue());
    }

    return QString("%1 %2 %3").arg(column.name(), sqlTypeString, sqlColumnOptions.join(" "));
}

} // namespace Helper
