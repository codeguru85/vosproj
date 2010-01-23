package dto.security.video
{
	import dto.security.video.VideoAlbumShipId;
	import dto.security.video.VideoInfo;
	import dto.security.video.VideoAlbum;
	[Bindable]
	[RemoteClass(alias="com.broadengate.vcms.entity.VideoAlbumShip")]
	public class VideoAlbumShip
	{
		public var id:VideoAlbumShipId;
		public var videoInfo:VideoInfo;
		public var videoAlbum:VideoAlbum;
		public function VideoAlbumShip()
		{
			//TODO: implement function
		}

	}
}