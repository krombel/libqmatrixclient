/******************************************************************************
 * Copyright (C) 2015 Felix Rohrbach <kde@fxrh.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include "jobs/generated/create_room.h"
#include "joinstate.h"

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QSize>

#include <functional>
#include <memory>

namespace QMatrixClient
{
    class Room;
    class User;
    class RoomEvent;
    class ConnectionData;

    class SyncJob;
    class SyncData;
    class RoomMessagesJob;
    class PostReceiptJob;
    class ForgetRoomJob;
    class MediaThumbnailJob;
    class JoinRoomJob;
    class UploadContentJob;
    class GetContentJob;
    class DownloadFileJob;

    class Connection: public QObject {
            Q_OBJECT

            /** Whether or not the rooms state should be cached locally
             * \sa loadState(), saveState()
             */
            Q_PROPERTY(User* localUser READ user CONSTANT)
            Q_PROPERTY(QString localUserId READ userId CONSTANT)
            Q_PROPERTY(QString deviceId READ deviceId CONSTANT)
            Q_PROPERTY(QByteArray accessToken READ accessToken CONSTANT)
            Q_PROPERTY(QUrl homeserver READ homeserver WRITE setHomeserver NOTIFY homeserverChanged)
            Q_PROPERTY(bool cacheState READ cacheState WRITE setCacheState NOTIFY cacheStateChanged)
        public:
            using room_factory_t =
                std::function<Room*(Connection*, const QString&, JoinState joinState)>;
            using user_factory_t =
                std::function<User*(Connection*, const QString&)>;

            enum RoomVisibility { PublishRoom, UnpublishRoom }; // FIXME: Should go inside CreateRoomJob

            explicit Connection(QObject* parent = nullptr);
            explicit Connection(const QUrl& server, QObject* parent = nullptr);
            virtual ~Connection();

            /** Get all Invited and Joined rooms
             * \return a hashmap from a composite key - room name and whether
             *         it's an Invite rather than Join - to room pointers
             */
            QHash<QPair<QString, bool>, Room*> roomMap() const;

            /** Get all Invited and Joined rooms grouped by tag
             * \return a hashmap from tag name to a vector of room pointers,
             *         sorted by their order in the tag - details are at
             *         https://matrix.org/speculator/spec/drafts%2Fe2e/client_server/unstable.html#id95
             */
            QHash<QString, QVector<Room*>> tagsToRooms() const;

            /** Get all room tags known on this connection */
            QStringList tagNames() const;

            /** Get the list of rooms with the specified tag */
            QVector<Room*> roomsWithTag(const QString& tagName) const;

            QMap<QString, User*> users() const;

            // FIXME: Convert Q_INVOKABLEs to Q_PROPERTIES
            // (breaks back-compatibility)
            QUrl homeserver() const;
            Q_INVOKABLE User* user(const QString& userId);
            User* user();
            QString userId() const;
            QString deviceId() const;
            /** @deprecated Use accessToken() instead. */
            Q_INVOKABLE QString token() const;
            QByteArray accessToken() const;
            Q_INVOKABLE SyncJob* syncJob() const;
            Q_INVOKABLE int millisToReconnect() const;

            /**
             * Call this before first sync to load from previously saved file.
             *
             * \param fromFile A local path to read the state from. Uses QUrl
             * to be QML-friendly. Empty parameter means using a path
             * defined by stateCachePath().
             */
            Q_INVOKABLE void loadState(const QUrl &fromFile = {});
            /**
             * This method saves the current state of rooms (but not messages
             * in them) to a local cache file, so that it could be loaded by
             * loadState() on a next run of the client.
             *
             * \param toFile A local path to save the state to. Uses QUrl to be
             * QML-friendly. Empty parameter means using a path defined by
             * stateCachePath().
             */
            Q_INVOKABLE void saveState(const QUrl &toFile = {}) const;

            /**
             * The default path to store the cached room state, defined as
             * follows:
             *     QStandardPaths::writeableLocation(QStandardPaths::CacheLocation) + _safeUserId + "_state.json"
             * where `_safeUserId` is userId() with `:` (colon) replaced with
             * `_` (underscore)
             * /see loadState(), saveState()
             */
            Q_INVOKABLE QString stateCachePath() const;

            bool cacheState() const;
            void setCacheState(bool newValue);

            /**
             * This is a universal method to start a job of a type passed
             * as a template parameter. Arguments to callApi() are arguments
             * to the job constructor _except_ the first ConnectionData*
             * argument - callApi() will pass it automatically.
             */
            template <typename JobT, typename... JobArgTs>
            JobT* callApi(JobArgTs&&... jobArgs) const
            {
                auto job = new JobT(std::forward<JobArgTs>(jobArgs)...);
                job->start(connectionData());
                return job;
            }

            /** Generates a new transaction id. Transaction id's are unique within
             * a single Connection object
             */
            Q_INVOKABLE QByteArray generateTxnId();

            template <typename T = Room>
            static void setRoomType()
            {
                roomFactory =
                    [](Connection* c, const QString& id, JoinState joinState)
                    { return new T(c, id, joinState); };
            }

            template <typename T = User>
            static void setUserType()
            {
                userFactory =
                    [](Connection* c, const QString& id) { return new T(id, c); };
            }

        public slots:
            /** Set the homeserver base URL */
            void setHomeserver(const QUrl& baseUrl);

            /** Determine and set the homeserver from domain or MXID */
            void resolveServer(const QString& mxidOrDomain);

            void connectToServer(const QString& user, const QString& password,
                                 const QString& initialDeviceName,
                                 const QString& deviceId = {});
            void connectWithToken(const QString& userId, const QString& accessToken,
                                  const QString& deviceId);

            /** @deprecated Use stopSync() instead */
            void disconnectFromServer() { stopSync(); }
            void logout();

            void sync(int timeout = -1);
            void stopSync();

            virtual MediaThumbnailJob* getThumbnail(const QString& mediaId,
                                                    QSize requestedSize) const;
            MediaThumbnailJob* getThumbnail(const QUrl& url,
                                            QSize requestedSize) const;
            MediaThumbnailJob* getThumbnail(const QUrl& url,
                                            int requestedWidth,
                                            int requestedHeight) const;

            // QIODevice* should already be open
            UploadContentJob* uploadContent(QIODevice* contentSource,
                            const QString& filename = {},
                            const QString& contentType = {}) const;
            UploadContentJob* uploadFile(const QString& fileName,
                                         const QString& contentType = {});
            GetContentJob* getContent(const QString& mediaId) const;
            GetContentJob* getContent(const QUrl& url) const;
            // If localFilename is empty, a temporary file will be created
            DownloadFileJob* downloadFile(const QUrl& url,
                            const QString& localFilename = {}) const;

            /**
             * \brief Create a room (generic method)
             * This method allows to customize room entirely to your liking,
             * providing all the attributes the original CS API provides.
             */
            CreateRoomJob* createRoom(RoomVisibility visibility,
                const QString& alias, const QString& name, const QString& topic,
                const QVector<QString>& invites, const QString& presetName = {}, bool isDirect = false,
                bool guestsCanJoin = false,
                const QVector<CreateRoomJob::StateEvent>& initialState = {},
                const QVector<CreateRoomJob::Invite3pid>& invite3pids = {},
                const QJsonObject creationContent = {});

            /** Create a direct chat with a single user, optional name and topic */
            CreateRoomJob* createDirectChat(const QString& userId,
                const QString& topic = {}, const QString& name = {});

            virtual JoinRoomJob* joinRoom(const QString& roomAlias);

            /** Sends /forget to the server and also deletes room locally.
             * This method is in Connection, not in Room, since it's a
             * room lifecycle operation, and Connection is an acting room manager.
             * It ensures that the local user is not a member of a room (running /leave,
             * if necessary) then issues a /forget request and if that one doesn't fail
             * deletion of the local Room object is ensured.
             * \param id - the room id to forget
             * \return - the ongoing /forget request to the server; note that the
             * success() signal of this request is connected to deleteLater()
             * of a respective room so by the moment this finishes, there might be no
             * Room object anymore.
             */
            ForgetRoomJob* forgetRoom(const QString& id);

            // Old API that will be abolished any time soon. DO NOT USE.

            /** @deprecated Use callApi<PostMessageJob>() or Room::postMessage() instead */
            virtual void postMessage(Room* room, const QString& type,
                                                 const QString& message) const;
            /** @deprecated Use callApi<PostReceiptJob>() or Room::postReceipt() instead */
            virtual PostReceiptJob* postReceipt(Room* room,
                                                RoomEvent* event) const;
            /** @deprecated Use callApi<LeaveRoomJob>() or Room::leaveRoom() instead */
            virtual void leaveRoom( Room* room );
            /** @deprecated User callApi<RoomMessagesJob>() or Room::getPreviousContent() instead */
            virtual RoomMessagesJob* getMessages(Room* room,
                                                 const QString& from) const;
        signals:
            /**
             * @deprecated
             * This was a signal resulting from a successful resolveServer().
             * Since Connection now provides setHomeserver(), the HS URL
             * may change even without resolveServer() invocation. Use
             * homeserverChanged() instead of resolved(). You can also use
             * connectToServer and connectWithToken without the HS URL set in
             * advance (i.e. without calling resolveServer), as they now trigger
             * server name resolution from MXID if the server URL is not valid.
             */
            void resolved();
            void resolveError(QString error);

            void homeserverChanged(QUrl baseUrl);

            void connected();
            void reconnected(); //< Unused; use connected() instead
            void loggedOut();
            void loginError(QString error);
            void networkError(int retriesTaken, int inMilliseconds);

            void syncDone();
            void syncError(QString error);

            void newUser(User* user);

            /**
             * \group Signals emitted on room transitions
             *
             * Note: Rooms in Invite state are always stored separately from
             * rooms in Join/Leave state, because of special treatment of
             * invite_state in Matrix CS API (see The Spec on /sync for details).
             * Therefore, objects below are: r - room in Join/Leave state;
             * i - room in Invite state
             *
             * 1. none -> Invite: newRoom(r), invitedRoom(r,nullptr)
             * 2. none -> Join: newRoom(r), joinedRoom(r,nullptr)
             * 3. none -> Leave: newRoom(r), leftRoom(r,nullptr)
             * 4. Invite -> Join:
             *      newRoom(r), joinedRoom(r,i), aboutToDeleteRoom(i)
             * 4a. Leave and Invite -> Join:
             *      joinedRoom(r,i), aboutToDeleteRoom(i)
             * 5. Invite -> Leave:
             *      newRoom(r), leftRoom(r,i), aboutToDeleteRoom(i)
             * 5a. Leave and Invite -> Leave:
             *      leftRoom(r,i), aboutToDeleteRoom(i)
             * 6. Join -> Leave: leftRoom(r)
             * 7. Leave -> Invite: newRoom(i), invitedRoom(i,r)
             * 8. Leave -> Join: joinedRoom(r)
             * The following transitions are only possible via forgetRoom()
             * so far; if a room gets forgotten externally, sync won't tell
             * about it:
             * 9. any -> none: as any -> Leave, then aboutToDeleteRoom(r)
             */

            /** A new room object has been created */
            void newRoom(Room* room);

            /** Invitation to a room received
             *
             * If the same room is in Left state, it's passed in prev.
             */
            void invitedRoom(Room* room, Room* prev);

            /** A room has just been joined
             *
             * It's not the same as receiving a room in "join" section of sync
             * response (rooms will be there even after joining). If this room
             * was in Invite state before, the respective object is passed in
             * prev (and it will be deleted shortly afterwards).
             */
            void joinedRoom(Room* room, Room* prev);

            /** A room has just been left
             *
             * If this room has been in Invite state (as in case of rejecting
             * an invitation), the respective object will be passed in prev
             * (and will be deleted shortly afterwards).
             */
            void leftRoom(Room* room, Room* prev);

            /** The room object is about to be deleted */
            void aboutToDeleteRoom(Room* room);

            /** The room has just been created by createRoom or createDirectChat
             * This signal is not emitted in usual room state transitions,
             * only as an outcome of room creation operations invoked by
             * the client.
             */
            void createdRoom(Room* room);

            void cacheStateChanged();

        protected:
            /**
             * @brief Access the underlying ConnectionData class
             */
            const ConnectionData* connectionData() const;

            /**
             * @brief Find a (possibly new) Room object for the specified id
             * Use this method whenever you need to find a Room object in
             * the local list of rooms. Note that this does not interact with
             * the server; in particular, does not automatically create rooms
             * on the server.
             * @return a pointer to a Room object with the specified id; nullptr
             * if roomId is empty if roomFactory() failed to create a Room object.
             */
            Room* provideRoom(const QString& roomId, JoinState joinState);

            /**
             * Completes loading sync data.
             */
            void onSyncSuccess(SyncData &&data);

        private:
            class Private;
            std::unique_ptr<Private> d;

            /**
             * A single entry for functions that need to check whether the
             * homeserver is valid before running. May either execute connectFn
             * synchronously or asynchronously (if tryResolve is true and
             * a DNS lookup is initiated); in case of errors, emits resolveError
             * if the homeserver URL is not valid and cannot be resolved from
             * userId.
             *
             * @param userId - fully-qualified MXID to resolve HS from
             * @param connectFn - a function to execute once the HS URL is good
             */
            void checkAndConnect(const QString& userId,
                                 std::function<void()> connectFn);
            void doConnectToServer(const QString& user, const QString& password,
                                   const QString& initialDeviceName,
                                   const QString& deviceId = {});

            static room_factory_t roomFactory;
            static user_factory_t userFactory;
    };
}  // namespace QMatrixClient
Q_DECLARE_METATYPE(QMatrixClient::Connection*)
