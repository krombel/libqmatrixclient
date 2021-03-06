/******************************************************************************
 * THIS FILE IS GENERATED - ANY EDITS WILL BE OVERWRITTEN
 */

#pragma once

#include "../basejob.h"

#include <QtCore/QVector>

#include "converters.h"

namespace QMatrixClient
{
    // Operations

    class GetPublicRoomsJob : public BaseJob
    {
        public:
            // Inner data structures

            struct PublicRoomsChunk
            {
                QVector<QString> aliases;
                QString canonicalAlias;
                QString name;
                double numJoinedMembers;
                QString roomId;
                QString topic;
                bool worldReadable;
                bool guestCanJoin;
                QString avatarUrl;
                
                operator QJsonObject() const;
            };

            // End of inner data structures

            /** Construct a URL out of baseUrl and usual parameters passed to
             * GetPublicRoomsJob. This function can be used when
             * a URL for GetPublicRoomsJob is necessary but the job
             * itself isn't.
             */
            static QUrl makeRequestUrl(QUrl baseUrl, double limit = {}, const QString& since = {}, const QString& server = {});

            explicit GetPublicRoomsJob(double limit = {}, const QString& since = {}, const QString& server = {});
            ~GetPublicRoomsJob() override;

            const QVector<PublicRoomsChunk>& chunk() const;
            const QString& nextBatch() const;
            const QString& prevBatch() const;
            double totalRoomCountEstimate() const;

        protected:
            Status parseJson(const QJsonDocument& data) override;

        private:
            class Private;
            QScopedPointer<Private> d;
    };

    class QueryPublicRoomsJob : public BaseJob
    {
        public:
            // Inner data structures

            struct Filter
            {
                QString genericSearchTerm;
                
                operator QJsonObject() const;
            };

            struct PublicRoomsChunk
            {
                QVector<QString> aliases;
                QString canonicalAlias;
                QString name;
                double numJoinedMembers;
                QString roomId;
                QString topic;
                bool worldReadable;
                bool guestCanJoin;
                QString avatarUrl;
                
                operator QJsonObject() const;
            };

            // End of inner data structures

            explicit QueryPublicRoomsJob(const QString& server = {}, double limit = {}, const QString& since = {}, const Filter& filter = {});
            ~QueryPublicRoomsJob() override;

            const QVector<PublicRoomsChunk>& chunk() const;
            const QString& nextBatch() const;
            const QString& prevBatch() const;
            double totalRoomCountEstimate() const;

        protected:
            Status parseJson(const QJsonDocument& data) override;

        private:
            class Private;
            QScopedPointer<Private> d;
    };
} // namespace QMatrixClient
