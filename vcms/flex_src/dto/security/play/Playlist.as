package dto.security.play
{
	import mx.collections.ArrayCollection;
	[Bindable]
	[RemoteClass(alias="com.broadengate.vcms.entity.Playlist")]
	public class Playlist
	{
		public var listId:int;
		public var listName:String;
		public var startTime:Date;
		public var endTime:Date;
		public var createTime:Date;
		public var playState:String;
		public var videoNum:int;
		public var playListShips:PlayListShip;
		public var termPlaylists:TermPlaylist;
		public function Playlist()
		{
		}

	}
}