package dto.security.play
{
	import dto.security.video.VideoInfo;
	[Bindable]
	[RemoteClass(alias="com.broadengate.vcms.entity.PlayListShip")]
	public class PlayListShip
	{
		public var shipId:int;
		public var playlist:Playlist;
		public var videoInfo:VideoInfo;
		public var createTime:Date;
		public var listSeq:String;
		public function PlayListShip()
		{
		}

	}
}